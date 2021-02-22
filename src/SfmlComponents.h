#ifndef SPEED_TYPER_SFML_COMPONENTS
#define SPEED_TYPER_SFML_COMPONENTS

#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/System/Vector2.hpp"
#include "WordsProvider.h"
#include <functional>
#include <string>
#include <vector>

namespace speedtyper {

class WordEntity : public sf::Drawable {
  public:
    WordEntity(float x, float y, const std::string& s, const sf::Font& font);

    enum class State {
        typed_correctly, // green letters
        typed_badly,     // red letters
        typing_ok,       // slightly highlighted
        typing_bad,      // in progress with error (red rectangle around)
        untouched        // displayed word which will be typed in the future
    };

    void set_state(State st);

    void set_position(float x, float y);
    void set_position(sf::Vector2f pos);

    auto get_position() const { return _text_border.getPosition(); }

    const std::string& get_string() const { return _s; }

    float get_width_of_bbox() const;

  private:
    void draw(sf::RenderTarget& target,
              sf::RenderStates states = sf::RenderStates::Default) const override;

    float _X{}; // _text and _text_border position
    float _Y{}; // _text and _text_border position
    std::string _s;
    sf::Text _text;
    sf::RectangleShape _text_border;
    State _state{State::untouched};
    constexpr static float _text_border_thickness{2.0F};
};

//------------------------------------------------------------------------------

class DisplayedWords : public sf::Drawable {

  public:
    explicit DisplayedWords(const sf::Font& font);

    const WordEntity& get_current_word() const {
        return _all_words.at(static_cast<std::vector<WordEntity>::size_type>(_current_word_idx));
    }

    WordEntity& get_current_word() {
        return _all_words.at(static_cast<std::vector<WordEntity>::size_type>(_current_word_idx));
    }

    void create_words();

    void move_all_words_line_up();

    void next_word(const std::string& previous);

    void update(const std::string& s);

    void reset();

  private:
    void draw(sf::RenderTarget& target,
              sf::RenderStates states = sf::RenderStates::Default) const override;

    const float _X{20.0F};
    const float _Y{20.0F};
    const float _Y_draw_boundary{100.0F};  // the bottom of the DisplayedWords (from the top of window)
    sf::Font _font;

    std::vector<WordEntity> _all_words;
    const int _num_words_ahead{1'000};
    int _current_word_idx{0};
    WordsProvider _wp{};
};

//------------------------------------------------------------------------------

class InputField : public sf::Drawable {
  public:
    explicit InputField(const sf::Font& font);

    auto get_position() const { return _input_field_bg.getPosition(); }

    auto get_size() const { return _input_field_bg.getSize(); }

    bool hover(const sf::Vector2i& v);

    bool is_active() const { return active; }

    void reset() {
        _input_field_bg.setFillColor(sf::Color::White);
        _input_field.setString("");
        set_active(true);
    }

    void set_active(bool b);

    void set_string(const std::string& s);

    void set_bg_color(const sf::Color& color);

  private:
    void draw(sf::RenderTarget& target,
              sf::RenderStates states = sf::RenderStates::Default) const override;

    const float _X{0.0F};
    const float _Y{0.0F};
    bool active{true};
    sf::Font _font;
    sf::Text _input_field;
    sf::RectangleShape _input_field_bg;
};

//------------------------------------------------------------------------------

class Button : public sf::Drawable {
  public:
    Button(sf::Vector2f pos, const std::string& text, const sf::Font& font,
           std::function<void()> func);

    bool hover(const sf::Vector2i& v);

    auto get_position() const { return _txt_bg.getPosition(); }

    auto get_size() const { return _txt_bg.getSize(); }

    void operator()() const { _callback(); }

  private:
    void draw(sf::RenderTarget& target,
              sf::RenderStates states = sf::RenderStates::Default) const override;
    float _X;
    float _Y;
    sf::Font _font;
    sf::Text _txt;
    sf::RectangleShape _txt_bg;
    std::function<void()> _callback;
};

//------------------------------------------------------------------------------

class TimerDisplay : public sf::Drawable {
  public:
    TimerDisplay(sf::Vector2f pos, int init_time_s, const sf::Font& font);

    bool hover(const sf::Vector2i& v);

    auto get_position() const { return _txt_bg.getPosition(); }

    auto get_size() const { return _txt_bg.getSize(); }

    void reset() {
        /* _txt_bg.setFillColor(sf::Color::Black); */
        _txt.setString("");
    }

    void toggl_visibility();

    void set_time_s(int time_s);

  private:
    float _X;
    float _Y;
    int _time_s;
    bool _visible;
    sf::Font _font;
    sf::Text _txt;
    sf::RectangleShape _txt_bg;

    std::string fmt_time_s_2_str(int time_s);

    void draw(sf::RenderTarget& target,
              sf::RenderStates states = sf::RenderStates::Default) const override;
};

} // namespace speedtyper

#endif /* ifndef SPEED_TYPER_SFML_COMPONENTS */
