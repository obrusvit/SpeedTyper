#include "SfmlComponents.h"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "constants.h"
#include <gsl/gsl-lite.hpp>
#include <utility>

namespace speedtyper {

    namespace {
        sf::RectangleShape create_bbox_around_text(const sf::Text& text, const sf::Font& font, unsigned int font_sz) {
            constexpr auto font_sz_expand_factor = 1.2F;
            constexpr auto text_border_thickness = 1.0F;
            auto pos_beg = text.findCharacterPos(0);
            auto pos_end = text.findCharacterPos(text.getString().getSize() - 1);
            gsl::span<const sf::Uint32> text_span{text.getString().getData(), text.getString().getSize()};
            auto last_letter = text_span.back();
            auto glyph = font.getGlyph(last_letter, font_sz, false);
            auto rect_width = pos_end.x - pos_beg.x + glyph.advance;
            auto rect_height = static_cast<float>(font_sz) * font_sz_expand_factor;
            sf::RectangleShape rect{sf::Vector2f{rect_width, rect_height}};
            rect.setPosition(text.getPosition());
            rect.setOutlineThickness(text_border_thickness);
            rect.setFillColor(sf::Color::Transparent);
            return rect;
        }
    }

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

//------------------------------------------------------------------------------


DisplayedWords::DisplayedWords(const sf::Font& font) : _font{font} {
    create_words();
}

void DisplayedWords::create_words(){
    float x_start = _X;
    float y_start = _Y;
    auto space_advance =
        _font.getGlyph(speedtyper::ASCII_values::key_space, speedtyper::GUI_options::FONT_SZ, false)
            .advance;
    for (int i = 0; i < _num_words_ahead; ++i) {
        auto word_entity = WordEntity(x_start, y_start, _wp.get_word(), _font);
        auto delta_x = word_entity.get_width_of_bbox() + space_advance;
        if (x_start + delta_x > static_cast<float>(speedtyper::GUI_options::win_sz_X)) {
            // this word spills out of main window so put it to new line
            x_start = _X;
            y_start += _font.getLineSpacing(speedtyper::GUI_options::FONT_SZ);
            word_entity.set_position(x_start, y_start);
        }
        x_start += delta_x;
        _all_words.push_back(word_entity);
    }
}

void DisplayedWords::update(const std::string& s) {
    auto& current_word = get_current_word();
    if (s == current_word.get_string().substr(0, s.size())) {
        current_word.set_state(WordEntity::State::typing_ok);
    } else {
        current_word.set_state(WordEntity::State::typing_bad);
    }
}

void DisplayedWords::next_word(const std::string& previous) {
    auto& current_word = get_current_word();
    if (current_word.get_string() == previous) {
        current_word.set_state(WordEntity::State::typed_correctly);
    } else {
        current_word.set_state(WordEntity::State::typed_badly);
    }
    ++_current_word_idx;
    auto& next_word = get_current_word();
    next_word.set_state(WordEntity::State::typing_ok);
    if (next_word.get_position().y > _Y) {
        move_all_words_line_up();
    }
}

void DisplayedWords::move_all_words_line_up() {
    for (auto& word : _all_words) {
        auto [x, y] = word.get_position();
        word.set_position(x, y - _font.getLineSpacing(speedtyper::GUI_options::FONT_SZ));
    }
}

void DisplayedWords::draw_word_collection(sf::RenderWindow& win) const {
    for (const auto& word_entity : _all_words) {
        auto y_pos = word_entity.get_position().y;
        if (y_pos <= _Y_draw_boundary) {
            word_entity.draw_word_entity(win);
        }
    }
}

void DisplayedWords::reset(){
    _all_words.clear();
    _current_word_idx = 0;
    create_words();
}

//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------

Button::Button(sf::Vector2f pos, const std::string& text, const sf::Font& font, std::function <void ()> func)
    : _X{pos.x}, _Y{pos.y}, _font{font}
    , _txt{text, font, speedtyper::GUI_options::FONT_SZ}
    , _txt_bg{}
    , _callback{std::move(func)}
{
    _txt_bg = create_bbox_around_text(_txt, _font, speedtyper::GUI_options::FONT_SZ);
    _txt_bg.setPosition(_X, _Y);
    _txt_bg.setFillColor(sf::Color::White);
    _txt_bg.setOutlineColor(sf::Color::Blue);
    _txt_bg.setOutlineThickness(1.0F);

    _txt.setFillColor(sf::Color::Blue);
    _txt.setPosition(_txt_bg.getPosition());

}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(_txt_bg);
    window.draw(_txt);
}

bool Button::hover(const sf::Vector2i& v) const {
    sf::Vector2f mousePosF{static_cast<float>(v.x), static_cast<float>(v.y)};
    return _txt_bg.getGlobalBounds().contains(mousePosF);
}

}  // namespace speedtyper
