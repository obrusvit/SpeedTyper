#include <string> 
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"

class WordEntity {
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

    void draw_word_entity(sf::RenderWindow& win) const;

    auto get_position() const { return _text_border.getPosition(); }

    const std::string& get_string() const { return _s; }

    float get_width_of_bbox() const;


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
                                               unsigned int font_sz) const;
};

