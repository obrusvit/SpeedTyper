#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Text.hpp"

class InputField {
    public:
    explicit InputField(const sf::Font& font);

    void draw_input_field(sf::RenderWindow& win) const;

    auto get_position() const { return _input_field_bg.getPosition(); }

    auto get_size() const { return _input_field_bg.getSize(); }

    void reset() { 
        _input_field_bg.setFillColor(sf::Color::White);
        _input_field.setString(""); 
    }

    void set_string(const std::string& s);

    void set_bg_color(const sf::Color& color);

    private:
    const float _X{0.0F};
    const float _Y{0.0F};
    sf::Font _font;
    sf::Text _input_field;
    sf::RectangleShape _input_field_bg;
};
