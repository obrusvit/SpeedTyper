#ifndef SPEED_TYPER_SCORE
#define SPEED_TYPER_SCORE

#include "constants.h"
#include "fmt/color.h"
#include "fmt/core.h"

#include <string>

namespace speedtyper {

struct Score {
    int words_correct{0};
    int words_bad{0};
    int chars_correct{0};
    int chars_bad{0};
    int backspaces{0};
    int spaces{0};
    int key_presses{0};
    int test_duration{1}; // in seconds

    Score() = default;

    // TODO assert test_duration is not zero or negative
    explicit Score(int t_time) : test_duration{t_time} {}

    void add_word(const std::string& added, const std::string& correct) {
        auto added_sz = static_cast<int>(added.size());
        if (added == correct) {
            chars_correct += added_sz;
            chars_correct += 1; // space
            words_correct += 1;
        } else {
            chars_bad += added_sz;
            chars_bad += 1; // space
            words_bad += 1;
        }
    }

    [[nodiscard]] double calculate_cpm() const {
        auto factor = 60.0 / test_duration;
        return chars_correct * factor;
    }

    [[nodiscard]] unsigned int calculate_wpm() const {
        auto cpm = calculate_cpm();
        return static_cast<unsigned int>(cpm / avg_word_len);
    }

    [[nodiscard]] double calculate_accuracy() const {
        return static_cast<double>(chars_correct) / (chars_correct + chars_bad);
    }

    // FIXME gcc 9.3 fails to compile default version; gcc 10.0 or clang 10.0 can generate it
    /* bool operator==(const Score&) const = default; */
    bool operator==(const Score& other) const {
        return words_correct == other.words_correct && words_bad == other.words_bad &&
               chars_correct == other.chars_correct && chars_bad == other.chars_bad &&
               backspaces == other.backspaces && spaces == other.spaces &&
               key_presses == other.key_presses && test_duration == other.test_duration;
    }

  private:
    static constexpr auto avg_word_len = 5;
};

} // namespace speedtyper

void show_results_terminal(const speedtyper::Score& score);

#endif /* ifndef SPEED_TYPER_SCORE */
