#include "DisplayedWords.h"
#include "constants.h"

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
