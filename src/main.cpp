#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <functional>
#include <gsl/gsl-lite.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <gsl/gsl-lite.hpp>
#include <utility>

#include "DisplayedWords.h"
#include "constants.h"



void handle_backspace(DisplayedWords& display, std::ostringstream& oss) {
    fmt::print("BS\n");
    auto so_far = oss.str();
    if (so_far.empty()) {
        return;
    }
    so_far.pop_back();
    oss.str(so_far);
    oss.clear();
    oss.seekp(0, std::ios_base::end); // set writing to the end
    display.update(oss.str());
}

void handle_space(DisplayedWords& display, std::ostringstream& oss) {
    display.next_word(oss.str());
    oss.str("");
    oss.clear();
}

void handle_written_char(DisplayedWords& display, std::ostringstream& oss, unsigned int unicode) {
    if (unicode > ST_ASCII_values::key_space && unicode < ST_ASCII_values::limit) {
        auto entered = static_cast<char>(unicode);
        fmt::print("ASCII char typed: {}\n", entered);
        oss << entered;
        display.update(oss.str());
    }
}

int main() {
    const unsigned int X = 1024;
    const unsigned int Y = 768;
    const unsigned int FONT_SZ = 30;
    const int FPS_LIMIT = 120;

    sf::RenderWindow window(sf::VideoMode(X, Y), "Typer++");
    window.setFramerateLimit(FPS_LIMIT);

    sf::Font font;
    if (!font.loadFromFile("../assets/ubuntu-font-family-0.83/"
                           "ubuntu-font-family-0.83/Ubuntu-L.ttf")) {
        fmt::print(fg(fmt::color::red), "font loading failed\n");
        return 1;
    }

    //------------------------------------------------------------------------------

    DisplayedWords displayed_words_collection{X, Y, font, FONT_SZ};

    //------------------------------------------------------------------------------

    sf::RectangleShape input_display_background{sf::Vector2f{X, FONT_SZ * 1.2}};
    input_display_background.setFillColor(sf::Color::White);
    input_display_background.setPosition(0, Y / 2.0);

    sf::Text input_display{"", font, FONT_SZ};
    input_display.setPosition(X / 10.0F, Y / 2.0F);
    input_display.setFillColor(sf::Color::Black);

    std::ostringstream oss{};

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered) {
                const auto unicode = event.text.unicode;
                switch (unicode) {
                case ST_ASCII_values::key_backspace:
                    handle_backspace(displayed_words_collection, oss);
                    break;
                case ST_ASCII_values::key_space:
                    handle_space(displayed_words_collection, oss);
                    break;
                default:
                    handle_written_char(displayed_words_collection, oss, unicode);
                    break;
                }
                input_display.setString(oss.str());
            }
        }

        window.clear();
        window.draw(input_display_background);
        window.draw(input_display);
        displayed_words_collection.draw_word_collection(window);
        window.display();
    }
}
