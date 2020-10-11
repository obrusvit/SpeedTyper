#include "WordEntity.h"
#include <gsl/gsl-lite.hpp>

WordEntity::WordEntity(float x, float y, const std::string& s, const sf::Font& font)
    : _X{x}, _Y{y}, _s{s}, _text{s, font, _font_sz} {
    _text.setPosition(_X, _Y);
    _text.setFillColor(sf::Color::Cyan);

    _text_border = create_bbox_around_text(_text, font, _font_sz);
    set_state(State::untouched);
}

void WordEntity::set_state(State st) {
    switch (st) {
    case State::typed_correctly:
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

void WordEntity::set_position(float x, float y) {
    _text.setPosition(x, y);
    _text_border.setPosition(x, y);
}

void WordEntity::set_position(sf::Vector2f pos) {
    _text.setPosition(pos);
    _text_border.setPosition(pos);
}

void WordEntity::draw_word_entity(sf::RenderWindow& win) const {
    win.draw(_text);
    win.draw(_text_border);
}

float WordEntity::get_width_of_bbox() const {
    auto text_border_sz = _text_border.getSize();
    return text_border_sz.x;
}

sf::RectangleShape WordEntity::create_bbox_around_text(const sf::Text& text, const sf::Font& font,
                                                       unsigned int font_sz) const {
    auto pos_beg = text.findCharacterPos(0);
    auto pos_end = text.findCharacterPos(text.getString().getSize() - 1);
    gsl::span<const sf::Uint32> text_span{_text.getString().getData(), _text.getString().getSize()};
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
