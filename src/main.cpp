#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <atomic>
#include <functional> // ref()
#include <stdexcept>
#include <thread>
#include <memory>
#include <utility>
#include <fmt/color.h>
#include <fmt/core.h>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Shape.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <X11/Xlib.h> // XInitThreads() otherwise error with [xcb]
#include <imgui.h>
#include <imgui-SFML.h>

#include "SFML/Window/Mouse.hpp"
#include "Score.h"
#include "SfmlComponents.h"
#include "Timer.h"
#include "constants.h"

void handle_backspace(speedtyper::Score& score, speedtyper::DisplayedWords& display,
                      std::ostringstream& oss) {
    fmt::print("BS\n");
    auto so_far = oss.str();
    if (so_far.empty()) {
        return;
    }
    so_far.pop_back();
    oss.str(so_far);
    oss.clear();
    oss.seekp(0, std::ios_base::end); // set writing to the end
    score.backspaces++;
    score.key_presses++;
    display.update(oss.str());
}

void handle_space(speedtyper::Score& score, speedtyper::DisplayedWords& display,
                  std::ostringstream& oss) {
    score.add_word(oss.str(), display.get_current_word().get_string());
    score.spaces++;
    score.key_presses++;
    display.next_word(oss.str());
    oss.str("");
    oss.clear();
}

void handle_written_char(speedtyper::Score& score, speedtyper::DisplayedWords& display,
                         std::ostringstream& oss, unsigned int unicode) {
    if (unicode > speedtyper::ASCII_values::key_space &&
        unicode < speedtyper::ASCII_values::limit) {
        auto entered = static_cast<char>(unicode);
        fmt::print("ASCII char typed: {}\n", entered);
        oss << entered;
        score.key_presses++;
        display.update(oss.str());
    }
}

void setup_ImGui(sf::RenderWindow& window){
    ImGui::SFML::Init(window, false);
    auto IO = ImGui::GetIO();
    IO.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
    IO.Fonts->AddFontFromFileTTF("../assets/dejavu-sans/DejaVuSans.ttf", 12.f);
    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture
}

void show_settings(int* test_time){
    ImGui::Begin("Settings");
    ImGui::SetWindowPos({100, 500});
    ImGui::Text("Just a text");
    ImGui::DragInt("test time", test_time, 1.0f, 10, 1200);
    ImGui::End();
}

enum class SpeedTyperStatus { waiting_for_start, running, finished, showing_results };


int main() {
    XInitThreads();
    sf::RenderWindow window(
        sf::VideoMode(speedtyper::GUI_options::win_sz_X, speedtyper::GUI_options::win_sz_Y),
        "Typer++");
    window.setFramerateLimit(speedtyper::GUI_options::FPS_LIMIT);
    setup_ImGui(window);

    sf::Font font;
    if (!font.loadFromFile("../assets/dejavu-sans/DejaVuSans.ttf")) {
        fmt::print(fg(fmt::color::red), "font loading failed\n");
        return 1;
    }

    //------------------------------------------------------------------------------

    std::ostringstream oss{};
    speedtyper::Timer timer;
    int timer_current_task_id = 0;
    std::atomic<SpeedTyperStatus> typer_status = SpeedTyperStatus::waiting_for_start;
    int test_time = speedtyper::gameopt::seconds_limit;  // [s]

    //------------------------------------------------------------------------------

    speedtyper::DisplayedWords displayed_words{font};
    speedtyper::InputField input_field{font};
    speedtyper::Score score{};
    std::vector<std::unique_ptr<sf::Drawable>> owning_drawables;
    auto reset_button_pos_x = input_field.get_position().x + input_field.get_size().x + 5.0F;
    auto reset_button_pos_y = input_field.get_position().y;
    auto reset_button_pos = sf::Vector2f{reset_button_pos_x, reset_button_pos_y};
    speedtyper::Button reset_button(reset_button_pos, "Reset", font, 
            [&]() {
                    displayed_words.reset();
                    input_field.reset();
                    score.reset();
                    oss.str("");
                    timer.disable(timer_current_task_id);
                    typer_status = SpeedTyperStatus::waiting_for_start;
                    owning_drawables.clear();
                }
            );
    std::vector<sf::Drawable*> not_owning_drawables {&input_field, &displayed_words, &reset_button};

    //------------------------------------------------------------------------------
    // delegate rendering to another thread

    
    sf::Clock deltaClock;
    window.setActive(false);
    auto rendering = [&window, &not_owning_drawables, &owning_drawables, &deltaClock, &test_time]() {
        window.setActive(true);
        while (window.isOpen()) {
            ImGui::SFML::Update(window, deltaClock.restart());
            show_settings(&test_time);
            window.clear();
            for (const auto& ptr_drawable : not_owning_drawables){
                window.draw(*ptr_drawable);
            }
            for (const auto& ptr_drawable : owning_drawables){
                window.draw(*ptr_drawable);
            }
            ImGui::SFML::Render(window);
            window.display();
        }
        ImGui::SFML::Shutdown();
    };
    std::thread render_thread{rendering};

    //------------------------------------------------------------------------------

    auto typing_loop_condition = [&typer_status, &input_field](const sf::Event& event) {
        return input_field.is_active() && event.type == sf::Event::TextEntered &&
               (typer_status == SpeedTyperStatus::running ||
                typer_status == SpeedTyperStatus::waiting_for_start);
    };

    auto func_timeout = [&typer_status]() { typer_status = SpeedTyperStatus::finished; };

    //------------------------------------------------------------------------------

    while (window.isOpen()) {
        sf::Event event{};
        while (window.waitEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (typer_status == SpeedTyperStatus::showing_results) {
                // intentionally empty for now
            }

            if (typer_status == SpeedTyperStatus::finished) {
                input_field.set_bg_color(sf::Color::Yellow);
                typer_status = SpeedTyperStatus::showing_results;
                show_results_terminal(score, test_time);

                auto result = fmt::format("Score is {}", score.calculate_wpm(test_time));
                auto* txt = new sf::Text(result, font, speedtyper::GUI_options::gui_font_sz);
                txt->setPosition(input_field.get_position().x+300, input_field.get_position().y+100);
                owning_drawables.push_back(std::unique_ptr<sf::Drawable>(txt));
            }

            // focus/unfocus the input field
            if (typer_status.load() == SpeedTyperStatus::waiting_for_start){
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (input_field.hover(sf::Mouse::getPosition(window))) {
                        input_field.set_active(true);
                    } else {
                        input_field.set_active(false);
                    }
                }
            }

            // handle buttons
            if (reset_button.hover(sf::Mouse::getPosition(window))) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    reset_button();
                }
            } 

            if (typing_loop_condition(event)) {
                const auto unicode = event.text.unicode;
                switch (unicode) {
                case speedtyper::ASCII_values::key_backspace:
                    handle_backspace(score, displayed_words, oss);
                    break;
                case speedtyper::ASCII_values::key_space:
                    handle_space(score, displayed_words, oss);
                    break;
                default:
                    if (typer_status == SpeedTyperStatus::waiting_for_start) {
                        using namespace std::chrono;
                        auto timeout = duration_cast<milliseconds>(
                            seconds(test_time));
                        typer_status = SpeedTyperStatus::running;
                        timer_current_task_id = timer.set_timeout(func_timeout, timeout);
                    }
                    handle_written_char(score, displayed_words, oss, unicode);
                    break;
                }
                input_field.set_string(oss.str());
            }
        }

    }
    render_thread.join();
    return 0;
}
