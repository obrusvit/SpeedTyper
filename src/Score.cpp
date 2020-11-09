#include "Score.h"

void show_results_terminal(const speedtyper::Score& score){
    /*
     * {[index]:[format_specifier]}
     * format_specifier:
     *     [[fill]align][sign][#]{0}[width][.precision][type]
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
    fmt::print(fg(fmt::color::gray),  "{0:.<{2}.2}{1:.>{3}}\n", "CPM:", score.calculate_cpm(), description_width, number_width);
    fmt::print(fg(fmt::color::gray) | fmt::emphasis::bold,  "{0:.<{2}}{1:.>{3}}\n", "WPM:", score.calculate_wpm(), description_width, number_width);
}
