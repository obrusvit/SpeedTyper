#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/System/Vector2.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <iostream>
#include <string>
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
#include <utility>
#include <gsl/gsl-lite.hpp>

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
    /* WordEntity() = default; */
    WordEntity(float x, float y, std::string s, const sf::Font& font, unsigned int font_sz) 
        : X{x} , Y{y} , _s{std::move(s)} , text{_s, font, font_sz}
    {
        text.setPosition(X, Y);
        text.setFillColor(sf::Color::Cyan);

        set_state(State::UNTOUCHED);

        auto pos_beg = text.findCharacterPos(0);
        auto pos_end = text.findCharacterPos(text.getString().getSize()-1);
        auto glyph = font.getGlyph(static_cast<sf::Uint32>(_s.back()), font_sz, false);
        auto pos_rect_x = pos_end.x - pos_beg.x + glyph.advance;
        auto pos_rect_y = static_cast<float>(font_sz*1.2);
        sf::RectangleShape rect {sf::Vector2f{pos_rect_x, pos_rect_y}};
        rect.setPosition(X, Y);
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2.0);
        rect.setFillColor(sf::Color::Transparent);
        text_border = rect;
    }

    enum class State { 
        TYPED_CORRECT,  // green letters
        TYPED_BADLY,    // red letters
        TYPING_OK,      // slightly highlighted
        TYPING_BAD,     // in progress with error (red rectangle around)
        UNTOUCHED       // displayed word which will be typed in the future
    };

    void set_state(State st) {
        state = st;
    }

    void draw_word_entity(sf::RenderWindow& win) {
        win.draw(text);
        win.draw(text_border);
        if (state == State::TYPING_BAD){
            text_border.setOutlineColor(sf::Color::Red);
            win.draw(text_border);
        }
    }

private:
    float X{};
    float Y{};
    std::string _s;
    sf::Text text;
    sf::RectangleShape text_border;
    State state {State::UNTOUCHED};
};


class DisplayedWords {
public:
    DisplayedWords(unsigned int X, unsigned int Y, const sf::Font& font, unsigned int font_sz)
        : wp{}, _X{X}, _Y{Y}, _font{font}, _font_sz{font_sz}
    {
        for(int i = 0; i < _num_words_ahead; ++i){
          const int test_offset_todel = 20;
          auto x = static_cast<float>(i * test_offset_todel);
          auto we = WordEntity(x, x, wp.get_word(), _font, _font_sz);
          all_words.push_back(we);
        }
        current_word = &all_words[0];
    }

    void draw_word_collection(sf::RenderWindow& win){
        for (auto& word_entity : all_words){
            word_entity.draw_word_entity(win);
        }
    }


private:
    WordsProvider wp;
    unsigned int _X;
    unsigned int _Y;
    sf::Font _font;
    unsigned int _font_sz;

    const int _num_words_ahead {30};
    std::vector<WordEntity> all_words;
    WordEntity* current_word{};
};


int main()
{
    const unsigned int X = 1024;
    const unsigned int Y = 768;
    const unsigned int FONT_SZ = 30;
    const int FPS_LIMIT = 120;

    sf::RenderWindow window(sf::VideoMode(X, Y), "Typer++");
    window.setFramerateLimit(FPS_LIMIT);

    sf::Font font;
    if ( ! font.loadFromFile("../assets/ubuntu-font-family-0.83/ubuntu-font-family-0.83/Ubuntu-L.ttf") ){
        fmt::print(fg(fmt::color::red), "font loading failed\n");
        return 1;
    }

    //------------------------------------------------------------------------------
    
    DisplayedWords displayed_words_collection {X, Y, font, FONT_SZ};
    /* WordEntity test_word_entity{10.0, 10.0, "Type this as fast as you can bitch", font, FONT_SZ}; */

    //------------------------------------------------------------------------------

    sf::RectangleShape input_display_background{sf::Vector2f{X, FONT_SZ*1.2}};
    input_display_background.setFillColor(sf::Color::White);
    input_display_background.setPosition(0, Y/2.0);

    sf::Text input_display{"", font, FONT_SZ};
    input_display.setPosition(X/10.0F, Y/2.0F);
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
        window.draw(input_display_background);
        window.draw(input_display);
        /* test_word_entity.draw_word_entity(window); */
        displayed_words_collection.draw_word_collection(window);
        window.display();
    }
}
