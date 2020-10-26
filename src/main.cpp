#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <atomic>
#include <chrono>
#include <fmt/color.h>
#include <fmt/core.h>
#include <functional>
#include <functional> // ref()
#include <gsl/gsl-lite.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <gsl/gsl-lite.hpp>
#include <utility>
#include <X11/Xlib.h>   // XInitThreads() otherwise error with [xcb]

#include "DisplayedWords.h"
#include "InputField.h"
#include "Score.h"
#include "constants.h"

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

void handle_written_char(Score& score, DisplayedWords& display, std::ostringstream& oss, unsigned int unicode) {
    if (unicode > speedtyper::ASCII_values::key_space &&
        unicode < speedtyper::ASCII_values::limit) {
        auto entered = static_cast<char>(unicode);
        fmt::print("ASCII char typed: {}\n", entered);
        oss << entered;
        score.key_presses++;
        display.update(oss.str());
    }
}

enum class SpeedTyperStatus { waiting_for_start, running, finished, showing_results };

void func_timer(std::atomic<SpeedTyperStatus>& ts) {
    using namespace std::chrono_literals;
    fmt::print("func_timer, start\n");
    std::chrono::seconds test_time{speedtyper::gameopt::seconds_limit};
    std::this_thread::sleep_for(test_time);
    ts = SpeedTyperStatus::finished;
    fmt::print("func_timer, done\n");
}

void show_results(const Score& score){
    /*
     * {[index]:[format_specifier]}
     * format_specifier:
     * [[fill]align][sign][#]{0}[width][.precision][type]
     */
    constexpr auto report_width = 30;
    constexpr auto number_width =  6;
    constexpr auto description_width =  report_width - number_width;
    fmt::print("\n{0:_^{1}}\n","Words", report_width);
    fmt::print(fg(fmt::color::green), "{0:.<{2}}{1:.>{3}}\n", "Correct:",   score.words_correct, description_width, number_width);
    fmt::print(fg(fmt::color::red),   "{0:.<{2}}{1:.>{3}}\n", "Incorrect:", score.words_bad, description_width, number_width);
    fmt::print("\n{0:_^{1}}\n", "Keys", report_width);
    auto chars_typed = fmt::format(fg(fmt::color::green), "{0}", score.chars_correct) 
        + fmt::format("|")
        + fmt::format(fg(fmt::color::red), "{0}", score.chars_bad);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}}\n", "Key presses:", chars_typed, description_width, number_width);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}.3}\n", "Accuracy:",  score.calculate_accuracy(), description_width, number_width);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}}\n", "Total key presses:",  score.key_presses, description_width, number_width);
    fmt::print(fg(fmt::color::red),   "{0:.<{2}}{1:.>{3}}\n", "Backspace:",   score.backspaces, description_width, number_width);
    fmt::print("\n{0:_^{1}}\n", "Summary", report_width);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}}\n", "CPM:", score.calculate_cpm(), description_width, number_width);
    fmt::print(fg(fmt::color::gray) | fmt::emphasis::bold,  "{0:.<{2}}{1:.>{3}}\n", "WPM:", score.calculate_wpm(), description_width, number_width);

}


void rendering_function(sf::RenderWindow& window, DisplayedWords& displayed_words, InputField& input_field, sf::RectangleShape& reset_button, sf::Text& reset_text) {
    // activate the window's context
    window.setActive(true);

    // the rendering loop
    while (window.isOpen()) {
        window.clear();
        input_field.draw_input_field(window);
        displayed_words.draw_word_collection(window);
        window.draw(reset_button);
        window.draw(reset_text);
        window.display();
    }
}


int main() {
    XInitThreads();  
    sf::RenderWindow window(
        sf::VideoMode(speedtyper::GUI_options::win_sz_X, speedtyper::GUI_options::win_sz_Y),
        "Typer++");
    window.setFramerateLimit(speedtyper::GUI_options::FPS_LIMIT);

    sf::Font font;
    if (!font.loadFromFile("../assets/ubuntu-font-family-0.83/"
                           "ubuntu-font-family-0.83/Ubuntu-L.ttf")) {
        fmt::print(fg(fmt::color::red), "font loading failed\n");
        return 1;
    }

    //------------------------------------------------------------------------------

    DisplayedWords displayed_words{font};
    InputField input_field{font};
    Score score{};

    //------------------------------------------------------------------------------
    sf::RectangleShape reset_button{sf::Vector2f{100, 50}};
    auto reset_button_pos_x = input_field.get_position().x + input_field.get_size().x + 5.0F;
    auto reset_button_pos_y = input_field.get_position().y;
    reset_button.setPosition(reset_button_pos_x, reset_button_pos_y);
    reset_button.setFillColor(sf::Color::White);
    reset_button.setOutlineColor(sf::Color::Blue);
    reset_button.setOutlineThickness(1.0F);

    sf::Text reset_text ("Reset", font, speedtyper::GUI_options::FONT_SZ);
    reset_text.setFillColor(sf::Color::Blue);
    reset_text.setPosition(reset_button.getPosition());


    // delegate rendering to another thread
    window.setActive(false);
    std::thread render_thread{
        rendering_function, 
        std::ref(window), 
        std::ref(displayed_words), 
        std::ref(input_field),
        std::ref(reset_button),
        std::ref(reset_text)};

    //------------------------------------------------------------------------------

    std::ostringstream oss{};
    std::thread timer_thread;
    speedtyper::Timer timer;
    std::atomic<SpeedTyperStatus> typer_status = SpeedTyperStatus::waiting_for_start;

    //------------------------------------------------------------------------------

    auto typing_loop_condition = [&typer_status](const sf::Event& event) {
        return event.type == sf::Event::TextEntered &&
               (typer_status == SpeedTyperStatus::running ||
                typer_status == SpeedTyperStatus::waiting_for_start);
    };

    auto reset_button_hover = [&window, &reset_button]() {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
        return reset_button.getGlobalBounds().contains(mousePosF);
    };

    auto reset_speed_typer = [&]() {
        displayed_words.reset();
        input_field.reset();
        score.reset();
        if (timer_thread.joinable()){
            timer_thread.detach();
        }
        // FIXME detached thread still have typer_status ref
        timer_thread = std::thread();
        typer_status = SpeedTyperStatus::waiting_for_start;
    };

    //------------------------------------------------------------------------------

    while (window.isOpen()) {
        sf::Event event{};
        while (window.waitEvent(event)) {
            if (event.type == sf::Event::Closed) {
                if (timer_thread.joinable()) {
                    // if the thread started already
                    timer_thread.detach();
                }
                window.close();
            }

            if (typer_status == SpeedTyperStatus::showing_results){
                // intentionally empty now
            }

            if (typer_status == SpeedTyperStatus::finished) {
                input_field.set_bg_color(sf::Color::Yellow);
                typer_status = SpeedTyperStatus::showing_results;
                show_results(score);
            }

            if (reset_button_hover()){
                reset_button.setFillColor(sf::Color(250, 20, 20));
                if (event.type == sf::Event::MouseButtonPressed) {
                    reset_speed_typer();
                }
            } else {
                reset_button.setFillColor(sf::Color::White);
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
                        typer_status = SpeedTyperStatus::running;
                        timer_thread = std::thread{func_timer, std::ref(typer_status)};
                    }
                    handle_written_char(score, displayed_words, oss, unicode);
                    break;
                }
                input_field.set_string(oss.str());
            }
        }
    }

    //------------------------------------------------------------------------------

    if (timer_thread.joinable()) {
        //prevent segfault if thread was detached because we closed the window before times up
        //prevent error if you close the window before typing anything
        timer_thread.join();
    }
    render_thread.join();

    return 0;
}
