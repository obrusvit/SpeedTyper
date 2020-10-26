#include "InputField.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/System/Vector2.hpp"
#include "constants.h"

InputField::InputField(const sf::Font& font)
    : _font{font}
    , _input_field{"", font, speedtyper::GUI_options::FONT_SZ}
    , _input_field_bg{sf::Vector2f{speedtyper::GUI_options::win_sz_X / 2.0F, speedtyper::GUI_options::FONT_SZ * 1.2}} 
    {
        _input_field_bg.setFillColor(sf::Color::White);
        _input_field_bg.setPosition(speedtyper::GUI_options::win_sz_X / 4.0F, speedtyper::GUI_options::win_sz_Y / 2.0F);

        _input_field.setPosition(speedtyper::GUI_options::win_sz_X / 4.0F, speedtyper::GUI_options::win_sz_Y / 2.0F);
        _input_field.setFillColor(sf::Color::Black);
    }

void InputField::draw_input_field(sf::RenderWindow& win) const {
    win.draw(_input_field_bg);
    win.draw(_input_field);
}

void InputField::set_string(const std::string& s){
    _input_field.setString(s);
}

void InputField::set_bg_color(const sf::Color& color){
    _input_field_bg.setFillColor(color);
}
