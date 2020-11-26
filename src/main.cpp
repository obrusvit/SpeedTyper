#include <algorithm>
#include <atomic>
#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>
#include <functional> // ref()
#include <iostream>
#include <matplot/axes_objects/box_chart.h>
#include <matplot/util/handle_types.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Shape.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <X11/Xlib.h> // XInitThreads() otherwise error with [xcb]
#include <imgui-SFML.h>
#include <imgui.h>

#include "DatabaseScore.h"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "Score.h"
#include "SfmlComponents.h"
#include "Timer.h"
#include "constants.h"

using namespace speedtyper;

void handle_backspace(Score& score, DisplayedWords& display, std::ostringstream& oss) {
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

void handle_space(Score& score, DisplayedWords& display, std::ostringstream& oss) {
    score.add_word(oss.str(), display.get_current_word().get_string());
    score.spaces++;
    score.key_presses++;
    display.next_word(oss.str());
    oss.str("");
    oss.clear();
}

void handle_written_char(Score& score, DisplayedWords& display, std::ostringstream& oss,
                         unsigned int unicode) {
    if (unicode > ASCII_values::key_space && unicode < ASCII_values::limit) {
        auto entered = static_cast<char>(unicode);
        fmt::print("ASCII char typed: {}\n", entered);
        oss << entered;
        score.key_presses++;
        display.update(oss.str());
    }
}

void setup_ImGui(sf::RenderWindow& window) {
    ImGui::SFML::Init(window, false);
    auto IO = ImGui::GetIO();
    IO.Fonts
        ->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
    IO.Fonts->AddFontFromFileTTF("../assets/dejavu-sans/DejaVuSans.ttf", 12.f);
    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture

    ImGuiStyle& style = ImGui::GetStyle();
    style.TabRounding = 0.0F;
    style.GrabRounding = 0.0F;
    style.FrameRounding = 0.0F;
    style.ChildRounding = 0.0F;
    style.PopupRounding = 0.0F;
    style.WindowRounding = 0.0F;
    style.ScrollbarRounding = 0.0F;
}

void show_settings(int* test_duration, bool* save_to_db) {
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::DragInt("test time", test_duration, 1.0f, gameopt::seconds_limit_min, gameopt::seconds_limit_max);
    ImGui::Checkbox("Save to db", save_to_db);
    ImGui::SetWindowPos({10, 500});
    /* ImGui::SetWindowSize({220, 100}); */
    ImGui::End();
}

#include <matplot/matplot.h>
void plot_with_matplotpp(PastData& past_data, const PastDataSetting& setting){
    auto data = past_data.get_past_data(setting);

    using namespace matplot;
    auto f = figure(true);
    /* auto ax = f->gca(); */
    auto ax = f->current_axes();
    auto p = ax->plot(data);
    p->color("blue").line_width(3);
    ax->ylabel(setting.get_what());
    ax->xlabel("There will be datetimes..");
    f->draw();
}

void show_past_results_plot(PastData& past_data) {
    static int n_results = 10;
    static std::array<int, 2> dur_min_max{10, 100};
    static const std::array<const char*, 5> items = {"WPM", "CPM", "words_correct", "words_bad",
                                                     "backspaces"};
    static int item_current = 0;
    ImGui::Begin("Past results", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::DragInt("Number of results", &n_results, 1.0f, 0, 10'000);
    ImGui::DragIntRange2("Min/Max dur", &dur_min_max.at(0), &dur_min_max.at(1), 1.0f,
                         gameopt::seconds_limit_min, gameopt::seconds_limit_max);
    ImGui::SameLine();
    ImGui::SameLine();
    ImGui::Combo("combo", &item_current, items.data(), items.size());

    const char* overlay = items.at(static_cast<decltype(items)::size_type>(item_current));
    PastDataSetting setting{std::string{overlay}, dur_min_max.at(0), dur_min_max.at(1), n_results};
    auto data = past_data.get_past_data(setting);
    auto scale_max = std::max_element(data.begin(), data.end());
    auto scale_min = std::min_element(data.begin(), data.end());
    if (!data.empty()) {
        ImGui::PlotLines("Past results", data.data(), static_cast<int>(data.size()), 0, overlay,
                         *scale_min, *scale_max, ImVec2(0, 140));
        if( ImGui::Button("Make proper plot.")){
            plot_with_matplotpp(past_data, setting);
        }
    } else {
        ImGui::Text("No results with given settings");
    }
    ImGui::SetWindowPos({240, 500});
    /* ImGui::SetWindowSize({550, 250}); */
    ImGui::End();
}

void show_results_imgui(const Score& score) {
    constexpr auto report_width = 30;
    constexpr auto number_width = 6;
    constexpr auto description_width = report_width - number_width;
    ImGui::Begin("Current result:", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::Separator();
    ImGui::Text("%s", fmt::format("{0}\n", "Words").c_str());
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s",
                       fmt::format("{0:.<{2}}{1:.>{3}}\n", "Correct:", score.words_correct,
                                   description_width, number_width)
                           .c_str());
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s",
                       fmt::format("{0:.<{2}}{1:.>{3}}\n", "Incorrect:", score.words_bad,
                                   description_width, number_width)
                           .c_str());
    auto chars_typed = fmt::format("{0}|{1}", score.chars_correct, score.chars_bad);

    ImGui::Separator();
    ImGui::Text("%s", fmt::format("{0}\n", "Keys").c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}}\n", "Chars typed:", chars_typed,
                                  description_width, number_width)
                          .c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}.3}\n", "Accuracy:", score.calculate_accuracy(),
                                  description_width, number_width)
                          .c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}}\n", "Total key presses:", score.key_presses,
                                  description_width, number_width)
                          .c_str());
    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s",
                       fmt::format("{0:.<{2}}{1:.>{3}}\n", "Backspace:", score.backspaces,
                                   description_width, number_width)
                           .c_str());

    ImGui::Separator();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", fmt::format("{0}\n", "Summary").c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}.2}{1:.>{3}}\n", "CPM:", score.calculate_cpm(),
                                  description_width, number_width)
                          .c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}}\n", "WPM:", score.calculate_wpm(),
                                  description_width, number_width)
                          .c_str());
    ImGui::SetWindowPos({800, 500});
    ImGui::End();
}

enum class SpeedTyperStatus { waiting_for_start, running, finished, showing_results };

int main() {
    XInitThreads();
    sf::RenderWindow window(sf::VideoMode(GUI_options::win_sz_X, GUI_options::win_sz_Y), "Typer++",
                            sf::Style::Close); // Close is to turn off resize ability
    window.setFramerateLimit(GUI_options::FPS_LIMIT);
    setup_ImGui(window);

    sf::Font font;
    if (!font.loadFromFile("../assets/dejavu-sans/DejaVuSans.ttf")) {
        fmt::print(fg(fmt::color::red), "font loading failed\n");
        return 1;
    }

    //------------------------------------------------------------------------------
    // State data

    std::ostringstream oss{};
    std::atomic<SpeedTyperStatus> typer_status = SpeedTyperStatus::waiting_for_start;
    Timer timer;
    auto timer_current_task_id = 0;
    auto test_duration = gameopt::seconds_limit_default; // [s]
    auto save_to_db = true;
    PastData past_data;

    //------------------------------------------------------------------------------
    // GUI objects

    DisplayedWords displayed_words{font};
    InputField input_field{font};
    Score score{test_duration};
    std::vector<std::unique_ptr<sf::Drawable>> owning_drawables;
    auto reset_button_pos_x = input_field.get_position().x + input_field.get_size().x + 5.0F;
    auto reset_button_pos_y = input_field.get_position().y;
    auto reset_button_pos = sf::Vector2f{reset_button_pos_x, reset_button_pos_y};
    Button reset_button(reset_button_pos, "Reset", font, [&]() {
        displayed_words.reset();
        input_field.reset();
        oss.str("");
        timer.disable(timer_current_task_id);
        typer_status = SpeedTyperStatus::waiting_for_start;
        owning_drawables.clear();
    });
    std::vector<sf::Drawable*> not_owning_drawables{&input_field, &displayed_words, &reset_button};

    //------------------------------------------------------------------------------
    // Delegate rendering to another thread

    sf::Clock deltaClock;
    window.setActive(false);
    auto rendering = [&]() {
        window.setActive(true);
        while (window.isOpen()) {
            ImGui::SFML::Update(window, deltaClock.restart());
            show_settings(&test_duration, &save_to_db);
            show_results_imgui(score);
            show_past_results_plot(past_data);
            window.clear();
            for (const auto& ptr_drawable : not_owning_drawables) {
                window.draw(*ptr_drawable);
            }
            for (const auto& ptr_drawable : owning_drawables) {
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

            if (event.type == sf::Event::Resized) {
                /* Resize everything accordingly */
                /* window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, */
                /*                 event.size.height))); */
            }

            if (typer_status == SpeedTyperStatus::showing_results) {
                // intentionally empty for now
            }

            if (typer_status == SpeedTyperStatus::finished) {
                input_field.set_bg_color(sf::Color::Yellow);
                typer_status = SpeedTyperStatus::showing_results;
                show_results_terminal(score);
                if (save_to_db) {
                    save_results_to_db(score);
                    past_data.invalidate();
                }

                auto result = fmt::format("Done, WPM is {}", score.calculate_wpm());
                auto txt = std::make_unique<sf::Text>(result, font, GUI_options::gui_font_sz);
                txt->setPosition(input_field.get_position().x,
                                 input_field.get_position().y + 50);
                owning_drawables.push_back(std::move(txt));
            }

            // focus/unfocus the input field
            if (typer_status.load() == SpeedTyperStatus::waiting_for_start) {
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
                case ASCII_values::key_backspace:
                    handle_backspace(score, displayed_words, oss);
                    break;
                case ASCII_values::key_space:
                    handle_space(score, displayed_words, oss);
                    break;
                default:
                    if (typer_status == SpeedTyperStatus::waiting_for_start) {
                        using namespace std::chrono;
                        auto timeout = duration_cast<milliseconds>(seconds(test_duration));
                        typer_status = SpeedTyperStatus::running;
                        score = Score(test_duration);
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
