#ifndef SPEED_TYPER_SCORE
#define SPEED_TYPER_SCORE

#include "constants.h"
#include <string>

struct Score {
    int words_correct{0};
    int words_bad{0};
    int chars_correct{0};
    int chars_bad{0};
    int backspaces{0};
    int spaces{0};
    int key_presses{0};

    void reset() {
        words_correct = 0;
        words_bad = 0;
        chars_correct = 0;
        chars_bad = 0;
        backspaces = 0;
        spaces = 0;
        key_presses = 0;
    }

    void add_word(const std::string& added, const std::string& correct) {
        auto added_sz = static_cast<int>(added.size());
        if (added == correct){
            chars_correct += added_sz;
            chars_correct += 1;  //space
            words_correct += 1;
        } else {
            chars_bad += added_sz;
            chars_bad += 1;  //space
            words_bad += 1;
        }
    }

    [[nodiscard]] double calculate_cpm() const {
        auto factor = 60.0 / speedtyper::gameopt::seconds_limit;
        return chars_correct * factor;
    }

    [[nodiscard]] unsigned int calculate_wpm() const {
        auto cpm = calculate_cpm();
        return static_cast<unsigned int> (cpm / avg_word_len);
    }

    [[nodiscard]] double calculate_accuracy() const {
        return static_cast<double>(chars_correct) / (chars_correct + chars_bad);
    }


    private:
        static constexpr auto avg_word_len = 5;
};

#endif /* ifndef SPEED_TYPER_SCORE */
