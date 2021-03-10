#include "SfmlComponents.h"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "constants.h"
#include <chrono>
#include <gsl/gsl-lite.hpp>
#include <utility>
#include <fmt/chrono.h>

namespace speedtyper {

namespace {
sf::RectangleShape create_bbox_around_text(const sf::Text& text, const sf::Font& font,
                                           unsigned int font_sz) {
    if (text.getString().isEmpty()) {
        return sf::RectangleShape{{0.0F, 0.0F}};
    }
    constexpr auto font_sz_expand_factor = 1.2F;
    constexpr auto text_border_thickness = 1.0F;
    auto pos_beg = text.findCharacterPos(0);
    auto pos_end = text.findCharacterPos(text.getString().getSize() - 1);
    auto last_letter = *(text.getString().end() - 1);
    auto rect_width = pos_end.x - pos_beg.x + font.getGlyph(last_letter, font_sz, false).advance;
    auto rect_height = static_cast<float>(font_sz) * font_sz_expand_factor;
    sf::RectangleShape rect{sf::Vector2f{rect_width, rect_height}};
    rect.setPosition(text.getPosition());
    rect.setOutlineThickness(text_border_thickness);
    rect.setFillColor(sf::Color::Transparent);
    return rect;
}
} // namespace

WordEntity::WordEntity(float x, float y, const std::string& s, const sf::Font& font)
    : _X{x}, _Y{y}, _s{s}, _text{s, font, GUI_options::ent_font_sz} {
    _text.setPosition(_X, _Y);
    _text.setFillColor(sf::Color::Cyan);

    _text_border = create_bbox_around_text(_text, font, GUI_options::ent_font_sz);
    set_state(State::untouched);
}

void WordEntity::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_text, states);
    target.draw(_text_border, states);
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

float WordEntity::get_width_of_bbox() const {
    auto text_border_sz = _text_border.getSize();
    return text_border_sz.x;
}

//------------------------------------------------------------------------------

DisplayedWords::DisplayedWords(const sf::Font& font) : _font{font} { create_words(); }

void DisplayedWords::create_words() {
    float x_start = _X;
    float y_start = _Y;
    auto space_advance =
        _font.getGlyph(ASCII_values::key_space, GUI_options::ent_font_sz, false).advance;
    for (int i = 0; i < _num_words_ahead; ++i) {
        auto word_entity = WordEntity(x_start, y_start, _wp.get_word(), _font);
        auto delta_x = word_entity.get_width_of_bbox() + space_advance;
        if (x_start + delta_x > static_cast<float>(GUI_options::win_sz_X)) {
            // this word spills out of main window so put it to new line
            x_start = _X;
            y_start += _font.getLineSpacing(GUI_options::ent_font_sz);
            word_entity.set_position(x_start, y_start);
        }
        x_start += delta_x;
        _all_words.push_back(word_entity);
    }
}

void DisplayedWords::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    for (const auto& word_entity : _all_words) {
        auto y_pos = word_entity.get_position().y;
        if (y_pos <= _Y_draw_boundary) {
            target.draw(word_entity, states);
        }
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
        word.set_position(x, y - _font.getLineSpacing(GUI_options::ent_font_sz));
    }
}

void DisplayedWords::reset() {
    _all_words.clear();
    _current_word_idx = 0;
    create_words();
}

//------------------------------------------------------------------------------

InputField::InputField(const sf::Font& font)
    : _font{font}, _input_field{"", font, GUI_options::gui_font_sz},
      _input_field_bg{sf::Vector2f{GUI_options::win_sz_X / 2.0F, GUI_options::gui_font_sz * 1.2}} {
    _input_field_bg.setPosition(GUI_options::win_sz_X / 5.0F, GUI_options::win_sz_Y / 3.0F);
    _input_field.setPosition(GUI_options::win_sz_X / 5.0F, GUI_options::win_sz_Y / 3.0F);
    set_active(true);
}

void InputField::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_input_field_bg, states);
    target.draw(_input_field, states);
}

bool InputField::hover(const sf::Vector2i& v) {
    sf::Vector2f mousePosF{static_cast<float>(v.x), static_cast<float>(v.y)};
    return _input_field_bg.getGlobalBounds().contains(mousePosF);
}

void InputField::set_active(bool b) {
    active = b;
    if (active) {
        _input_field_bg.setFillColor(sf::Color::White);
        _input_field.setFillColor(sf::Color::Black);
    } else {
        _input_field_bg.setFillColor(sf::Color(200, 200, 200));
        _input_field.setFillColor(sf::Color::Red);
    }
}

void InputField::set_string(const std::string& s) { _input_field.setString(s); }

void InputField::set_bg_color(const sf::Color& color) { _input_field_bg.setFillColor(color); }

//------------------------------------------------------------------------------

Button::Button(sf::Vector2f pos, const std::string& text, const sf::Font& font,
               std::function<void()> func)
    : _X{pos.x}, _Y{pos.y}, _font{font}, _txt{text, font, GUI_options::gui_font_sz}, _txt_bg{},
      _callback{std::move(func)} {
    _txt_bg = create_bbox_around_text(_txt, _font, GUI_options::gui_font_sz);
    _txt_bg.setPosition(_X, _Y);
    _txt_bg.setFillColor(sf::Color::White);
    _txt_bg.setOutlineColor(sf::Color::Blue);
    _txt_bg.setOutlineThickness(1.0F);

    _txt.setFillColor(sf::Color::Black);
    _txt.setPosition(_txt_bg.getPosition());
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_txt_bg, states);
    target.draw(_txt, states);
}

bool Button::hover(const sf::Vector2i& v) {
    sf::Vector2f mousePosF{static_cast<float>(v.x), static_cast<float>(v.y)};
    if (_txt_bg.getGlobalBounds().contains(mousePosF)) {
        _txt_bg.setFillColor(sf::Color(255, 25, 25, 255));
        return true;
    }
    _txt_bg.setFillColor(sf::Color::White);
    return false;
}

//------------------------------------------------------------------------------

TimerDisplay::TimerDisplay(sf::Vector2f pos, int init_time_s, const sf::Font& font)
    : _X{pos.x}, _Y{pos.y}, _time_s{init_time_s},
      _visible{true}, _font{font}, _txt{"", font, GUI_options::gui_font_sz}, _txt_bg{} {
    _txt.setString(fmt_time_s_2_str(_time_s));

    _txt_bg = create_bbox_around_text(_txt, _font, GUI_options::gui_font_sz);
    _txt_bg.setPosition(_X, _Y);
    _txt_bg.setFillColor(sf::Color::White);
    _txt_bg.setOutlineColor(sf::Color::Blue);
    _txt_bg.setOutlineThickness(1.0F);

    _txt.setFillColor(sf::Color::Black);
    _txt.setPosition(_txt_bg.getPosition());
}

bool TimerDisplay::hover(const sf::Vector2i& v){
    sf::Vector2f mousePosF{static_cast<float>(v.x), static_cast<float>(v.y)};
    if (_txt_bg.getGlobalBounds().contains(mousePosF)) {
        // 
        return true;
    }
    return false;
}

void TimerDisplay::toggl_visibility(){
    if(_visible){
        _txt_bg.setFillColor(sf::Color::White);
        _txt.setFillColor(sf::Color::White);
        _visible = false;
    } else {
        _txt_bg.setFillColor(sf::Color::White);
        _txt.setFillColor(sf::Color::Black);
        _visible = true;
    }
}

void TimerDisplay::set_time_s(int time_s){
    _time_s = time_s;
    _txt.setString(fmt_time_s_2_str(time_s));
}

void TimerDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(_txt_bg, states);
    target.draw(_txt, states);
}

std::string TimerDisplay::fmt_time_s_2_str(int time_s){
    return fmt::format("{:%M:%S}", std::chrono::seconds{time_s});
    /* return fmt::format("{}", std::chrono::seconds{time_s}.count()); */
}

} // namespace speedtyper
