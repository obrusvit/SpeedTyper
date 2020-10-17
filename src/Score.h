#ifndef SPEED_TYPER_SCORE
#define SPEED_TYPER_SCORE

#include "constants.h"
#include <string>

struct Score {
    int words_correct{0};
    int words_bad{0};
    int backspaces{0};
    int key_presses{0};

    void add_word(const std::string& added, const std::string& correct) {
        (added == correct) ? words_correct++ : words_bad++;
    }

    [[nodiscard]] unsigned int calculate_wpm() const {
        return words_correct * (60 / speedtyper::gameopt::seconds_limit);
    }
};

#endif /* ifndef SPEED_TYPER_SCORE */
