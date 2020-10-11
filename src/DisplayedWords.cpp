#include "DisplayedWords.h"
#include "constants.h"

DisplayedWords::DisplayedWords(unsigned int X, unsigned int Y, const sf::Font& font, unsigned int font_sz)
    : _win_X{X}, _win_Y{Y}, _font{font}, _font_sz{font_sz} {
    float x_start = _X;
    float y_start = _Y;
    auto space_advance = _font.getGlyph(ST_ASCII_values::key_space, _font_sz, false).advance;
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
        word.set_position(x, y - _font.getLineSpacing(_font_sz));
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
