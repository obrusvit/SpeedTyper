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

#include "words_provider.hpp"

constexpr int ascii_backspace = 8;
constexpr int ascii_space = 32;
constexpr int ascii_limit = 126;



class WordEntity {
  public:
    WordEntity(float x, float y, const std::string& s, const sf::Font& font)
        : _X{x}, _Y{y}, _s{s}, _text{s, font, _font_sz} {
        _text.setPosition(_X, _Y);
        _text.setFillColor(sf::Color::Cyan);

        _text_border = create_bbox_around_text(_text, font, _font_sz);
        set_state(State::untouched);
    }

    enum class State {
        typed_correct, // green letters
        typed_badly,   // red letters
        typing_ok,     // slightly highlighted
        typing_bad,    // in progress with error (red rectangle around)
        untouched      // displayed word which will be typed in the future
    };

    void set_state(State st) { 
        switch (st) {
        case State::typed_correct:
            _text.setFillColor(sf::Color::Green);
            _text_border.setOutlineColor(sf::Color::Transparent);
            break;
        case State::typed_badly:
            _text.setFillColor(sf::Color::Red);
            _text_border.setOutlineColor(sf::Color::Transparent);
            break;
        case State::typing_ok:
            _text.setFillColor(sf::Color::White);
            _text_border.setOutlineColor(sf::Color(179, 217, 255));
            break;
        case State::typing_bad:
            _text.setFillColor(sf::Color::Red);
            _text_border.setOutlineColor(sf::Color::Red);
            break;
        case State::untouched:
            _text.setFillColor(sf::Color::White);
            _text_border.setOutlineColor(sf::Color::Transparent);
            break;
        }
        _state = st; 
    }

    void set_position(float x, float y) {
        _text.setPosition(x, y);
        _text_border.setPosition(x, y);
    }

    void set_position(sf::Vector2f pos) {
        _text.setPosition(pos);
        _text_border.setPosition(pos);
    }

    void draw_word_entity(sf::RenderWindow& win) const {
        win.draw(_text);
        win.draw(_text_border);
    }

    auto get_position() const { return _text_border.getPosition(); }

    const std::string& get_string() const { return _s; }

    float get_width_of_bbox() const {
        auto text_border_sz = _text_border.getSize();
        return text_border_sz.x;
    }


  private:
    float _X{};
    float _Y{};
    std::string _s;
    sf::Text _text;
    sf::RectangleShape _text_border;
    State _state{State::untouched};
    constexpr static unsigned int _font_sz{30};
    constexpr static float _font_sz_expand_factor{1.2F};
    constexpr static float _text_border_thickness{2.0F};

    sf::RectangleShape create_bbox_around_text(const sf::Text& text, const sf::Font& font,
                                               unsigned int font_sz) const {
        auto pos_beg = text.findCharacterPos(0);
        auto pos_end = text.findCharacterPos(text.getString().getSize() - 1);
        gsl::span<const sf::Uint32> text_span{_text.getString().getData(),
                                              _text.getString().getSize()};
        auto last_letter = text_span.back();
        auto glyph = font.getGlyph(last_letter, _font_sz, false);
        auto rect_width = pos_end.x - pos_beg.x + glyph.advance;
        auto rect_height = static_cast<float>(font_sz) * _font_sz_expand_factor;
        sf::RectangleShape rect{sf::Vector2f{rect_width, rect_height}};
        rect.setPosition(_X, _Y);
        rect.setOutlineThickness(_text_border_thickness);
        rect.setFillColor(sf::Color::Transparent);
        return rect;
    }
};


class DisplayedWords {

  public:
    DisplayedWords(unsigned int X, unsigned int Y, const sf::Font& font, unsigned int font_sz)
        : _win_X{X}, _win_Y{Y}, _font{font}, _font_sz{font_sz} {
        float x_start = _X;
        float y_start = _Y;
        auto space_advance = _font.getGlyph(ascii_space, _font_sz, false).advance;
        for (int i = 0; i < _num_words_ahead; ++i) {
            auto word_entity = WordEntity(x_start, y_start, _wp.get_word(), _font);
            auto delta_x = word_entity.get_width_of_bbox() + space_advance;
            if (x_start + delta_x > static_cast<float>(_win_X)) {
                // this word spills out of main window so put it to new line
                x_start = _X;
                y_start += _font.getLineSpacing(font_sz);
                word_entity.set_position(x_start, y_start);
            }
            x_start += delta_x;
            _all_words.push_back(word_entity);
        }
    }

    const WordEntity& get_current_word() const {
        return _all_words.at(static_cast<std::vector<WordEntity>::size_type>(_current_word_idx));
    }

    WordEntity& get_current_word() {
        return _all_words.at(static_cast<std::vector<WordEntity>::size_type>(_current_word_idx));
    }

    const std::string& get_current_word_string() const {
        return get_current_word().get_string();
    }

    void update(const std::string& s) const {
        auto current_word = get_current_word();
        if (s == current_word.get_string().substr(0, s.size())){
            current_word.set_state(WordEntity::State::typing_ok);
        } else {
            current_word.set_state(WordEntity::State::typing_bad);
        } 
    }

    void next_word(const std::string& previous){
        auto current_word = get_current_word();
        if (current_word.get_string() == previous){
            current_word.set_state(WordEntity::State::typed_correct);
        } else {
            current_word.set_state(WordEntity::State::typing_bad);
        }
        ++_current_word_idx;
    }

    void draw_word_collection(sf::RenderWindow& win) const {
        for (const auto& word_entity : _all_words) {
            auto y_pos = word_entity.get_position().y;
            if (y_pos <= _Y_draw_boundary) {
                word_entity.draw_word_entity(win);
            }
        }
    }

  private:
    unsigned int _win_X;
    unsigned int _win_Y;
    const float _X{20.0F};
    const float _Y{20.0F};
    const float _Y_draw_boundary{200.0F};
    sf::Font _font;
    unsigned int _font_sz;

    std::vector<WordEntity> _all_words;
    const int _num_words_ahead{90};
    int _current_word_idx{0};
    WordsProvider _wp{};
};

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
    if (unicode > ascii_space && unicode < ascii_limit) {
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
                case ascii_backspace:
                    handle_backspace(displayed_words_collection, oss);
                    break;
                case ascii_space:
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
