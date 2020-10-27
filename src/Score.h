#ifndef SPEED_TYPER_SCORE
#define SPEED_TYPER_SCORE

#include "constants.h"
#include "fmt/core.h"
#include "fmt/color.h"

#include <string>

namespace speedtyper{

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

}  // namespace speedtyper

void show_results(const speedtyper::Score& score){
    /*
     * {[index]:[format_specifier]}
     * format_specifier:
     * [[fill]align][sign][#]{0}[width][.precision][type]
     */
    constexpr auto report_width = 30;
    constexpr auto number_width =  6;
    constexpr auto description_width =  report_width - number_width;
    fmt::print("\n{0:_^{1}}\n","Words", report_width);
    fmt::print(fg(fmt::color::green), "{0:.<{2}}{1:.>{3}}\n", "Correct:",   score.words_correct, description_width, number_width);
    fmt::print(fg(fmt::color::red),   "{0:.<{2}}{1:.>{3}}\n", "Incorrect:", score.words_bad, description_width, number_width);
    fmt::print("\n{0:_^{1}}\n", "Keys", report_width);
    auto chars_typed = fmt::format(fg(fmt::color::green), "{0}", score.chars_correct) 
        + fmt::format("|")
        + fmt::format(fg(fmt::color::red), "{0}", score.chars_bad);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}}\n", "Chars typed:", chars_typed, description_width, number_width);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}.3}\n", "Accuracy:",  score.calculate_accuracy(), description_width, number_width);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}}\n", "Total key presses:",  score.key_presses, description_width, number_width);
    fmt::print(fg(fmt::color::red),   "{0:.<{2}}{1:.>{3}}\n", "Backspace:",   score.backspaces, description_width, number_width);
    fmt::print("\n{0:_^{1}}\n", "Summary", report_width);
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}}{1:.>{3}}\n", "CPM:", score.calculate_cpm(), description_width, number_width);
    fmt::print(fg(fmt::color::gray) | fmt::emphasis::bold,  "{0:.<{2}}{1:.>{3}}\n", "WPM:", score.calculate_wpm(), description_width, number_width);
}


#endif /* ifndef SPEED_TYPER_SCORE */
