#include <vector>
#include "SFML/Graphics/Font.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "WordsProvider.h"
#include "WordEntity.h"

class DisplayedWords {

  public:
    DisplayedWords(unsigned int X, unsigned int Y, const sf::Font& font, unsigned int font_sz);

    const WordEntity& get_current_word() const {
        return _all_words.at(static_cast<std::vector<WordEntity>::size_type>(_current_word_idx));
    }

    WordEntity& get_current_word() {
        return _all_words.at(static_cast<std::vector<WordEntity>::size_type>(_current_word_idx));
    }

    void update(const std::string& s); 

    void next_word(const std::string& previous);

    void move_all_words_line_up(); 

    void draw_word_collection(sf::RenderWindow& win) const;

  private:
    unsigned int _win_X;
    unsigned int _win_Y;
    const float _X{20.0F};
    const float _Y{20.0F};
    const float _Y_draw_boundary{200.0F};
    sf::Font _font;
    unsigned int _font_sz;

    std::vector<WordEntity> _all_words;
    const int _num_words_ahead{1'000};
    int _current_word_idx{0};
    WordsProvider _wp{};
};
