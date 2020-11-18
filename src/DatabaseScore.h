#ifndef SPEEDTYPER_DATABASE_UTIL_FUNCTIONS
#define SPEEDTYPER_DATABASE_UTIL_FUNCTIONS value

#include "Score.h"
#include "TabScore.h"
#include <sqlpp11/custom_query.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>
#include <vector>
/* #include <concepts> */

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif

/* SQLPP_ALIAS_PROVIDER(left) */
/* SQLPP_ALIAS_PROVIDER(pragma) */
/* SQLPP_ALIAS_PROVIDER(sub) */

namespace speedtyper {


void print_rows_tabscore(auto& db, const auto& tab) {
    // explicit all_of(tab)
    fmt::print("Printing all rows TabScore:\n");
    for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally())) {
        std::cout << "row id " << row.id << "\n\t"
                  << "words_correct: " << row.words_correct << ", "
                  << "words_bad: " << row.words_bad << ", "
                  << "chars_correct: " << row.chars_correct << ", "
                  << "chars_bad: " << row.chars_bad << ", "
                  << "backspaces: " << row.backspaces << ", "
                  << "spaces: " << row.spaces << ", "
                  << "key_presses: " << row.key_presses << ", "
                  << "wpm: " << row.wpm << ", "
                  << "cpm: " << row.cpm << ", "
                  << "test_duration: " << row.test_duration << ", "
                  << "test_datetime: " << row.test_datetime << ", "
                  << "\n";
    };
    fmt::print("---\n");
}

auto row_to_score(const auto& row) {
    auto ret = speedtyper::Score(static_cast<int>(row.test_duration));
    ret.words_correct = static_cast<int>(row.words_correct);
    ret.words_bad = static_cast<int>(row.words_bad);
    ret.chars_correct = static_cast<int>(row.chars_correct);
    ret.chars_bad = static_cast<int>(row.chars_bad);
    ret.backspaces = static_cast<int>(row.backspaces);
    ret.spaces = static_cast<int>(row.spaces);
    ret.key_presses = static_cast<int>(row.key_presses);
    return ret;
}

inline auto create_insert_score_query(const TabScore& tab, speedtyper::Score s1) {
    const auto now = std::chrono::system_clock::now();
    return insert_into(tab).set(tab.words_correct = s1.words_correct, tab.words_bad = s1.words_bad,
                                tab.chars_correct = s1.chars_correct, tab.chars_bad = s1.chars_bad,
                                tab.backspaces = s1.backspaces, tab.spaces = s1.spaces,
                                tab.key_presses = s1.key_presses, tab.wpm = s1.calculate_wpm(),
                                tab.cpm = s1.calculate_cpm(), tab.test_duration = s1.test_duration,
                                tab.test_datetime = now);
}

void save_results_to_db(const speedtyper::Score&);

sqlpp::sqlite3::connection get_db_connection();

template<typename T> 
    // requires std::floating_point<T>
std::vector<T> get_all_wpm_from_db(){
    std::vector<T> ret{};
    auto db = get_db_connection();
    const auto tab = TabScore{};
    auto result = db(select(tab.wpm).from(tab).unconditionally());
    for (const auto& row : result){
        ret.push_back(static_cast<T>(row.wpm));
    }
    return ret;
}

}  // namespace speedtyper
#endif /* ifndef SPEEDTYPER_DATABASE_UTIL_FUNCTIONS */
