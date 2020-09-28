#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fmt/core.h>
#include <fmt/color.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include "words_provider.hpp"

void handle_backspace(std::ostringstream& oss){
    fmt::print("BS\n");
    auto so_far = oss.str();
    if (! so_far.empty()) {
        so_far.pop_back();
        oss.str(so_far);
        oss.clear();
        oss.seekp(0, std::ios_base::end);  //set writing to the end
        fmt::print("{}\n", oss.tellp());
    }
}

void handle_space(std::ostringstream& oss) {
    oss.str("");
    oss.clear();
}

void handle_written_char(std::ostringstream& oss, unsigned int unicode){
    if (unicode > 32 && unicode < 128) {
        auto entered = static_cast<char>(unicode);
        fmt::print("ASCII char typed: {}\n", entered);
        oss << entered;
    }
}


class WordEntity {
public:
    void mark_faulty();
    void mark_OK();

private:
    std::string _s;
    sf::Vector2f _bbox;
    
};


class DisplayedWords {
public:


private:
    std::vector<WordEntity> _words;
};


int main()
{
    const unsigned int X = 1024;
    const unsigned int Y = 768;
    const int FPS_LIMIT = 120;
    const int FONT_SZ = 30;

    const WordsProvider words_provider{};

    sf::RenderWindow window(sf::VideoMode(X, Y), "Typer++");
    window.setFramerateLimit(FPS_LIMIT);

    sf::Font font;
    if ( ! font.loadFromFile("../assets/ubuntu-font-family-0.83/ubuntu-font-family-0.83/Ubuntu-L.ttf") ){
        fmt::print(fg(fmt::color(fmt::color::red)), "font loading failed\n");
        return 1;
    }

    const auto& word = words_provider.get_word();
    sf::Text text{word, font, FONT_SZ};
    text.setFillColor(sf::Color::Red);
    text.setPosition(10.0, 10.0);

    auto pf = text.findCharacterPos(0);
    auto pl = text.findCharacterPos(word.size()-1);
    fmt::print("pos of first: {}, {}, pos of last: {}, {}\n", pf.x, pf.y, pl.x, pl.y);

    auto glyph = font.getGlyph(word.back(), FONT_SZ, false);
    auto info = font.getInfo();
    font.getUnderlinePosition(30);
    
    fmt::print("glyph: {}\n", glyph.advance);
    sf::RectangleShape rect_behind_word{sf::Vector2f{pl.x - pf.x + glyph.advance, FONT_SZ}};
    rect_behind_word.setPosition(pf);
    /* rect_behind_word.setOutlineThickness(1.f); */
    /* rect_behind_word.setOutlineColor(sf::Color::Cyan); */
    /* rect_behind_word.setFillColor(sf::Color::Transparent); */
    rect_behind_word.setFillColor(sf::Color(255, 0, 0, 50));


    sf::RectangleShape input_display_background{sf::Vector2f{X, FONT_SZ*1.2}};
    input_display_background.setFillColor(sf::Color::White);
    input_display_background.setPosition(0, Y/2.0);

    sf::Text input_display{"", font, FONT_SZ};
    input_display.setPosition(X/10.0, Y/2.0);
    /* input_display.setStyle(sf::Text::StrikeThrough); */
    input_display.setOutlineColor(sf::Color::Green);
    input_display.setFillColor(sf::Color::Blue);


    std::ostringstream oss{};

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::TextEntered){
                const int BACKSPACE = 8;
                const int SPACE = 32;
                const auto unicode = event.text.unicode;
                switch (unicode) {
                    case BACKSPACE:  // '\b'
                        handle_backspace(oss);
                        break;
                    case SPACE: 
                        handle_space(oss);
                        break;
                    default:
                        handle_written_char(oss, unicode);
                        break;
                }
            input_display.setString(oss.str());    
            }
        }

        window.clear();
        window.draw(text);
        window.draw(input_display_background);
        window.draw(input_display);
        window.draw(rect_behind_word);
        window.display();
    }
}
