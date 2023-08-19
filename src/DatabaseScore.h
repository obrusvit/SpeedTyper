#ifndef SPEEDTYPER_DATABASE_UTIL_FUNCTIONS
#define SPEEDTYPER_DATABASE_UTIL_FUNCTIONS value

#include "Score.h"
#include "TabScore.h"
#include <chrono>
#include <fmt/color.h>
#include <ios>
#include <sqlpp11/custom_query.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>
#include <tuple>
#include <utility>
#include <vector>

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif

/* SQLPP_ALIAS_PROVIDER(left) */
/* SQLPP_ALIAS_PROVIDER(pragma) */
/* SQLPP_ALIAS_PROVIDER(sub) */

namespace speedtyper {

// many of these functions are defined in this header because otherwise,
// there would have to be insanely long template types instead of 'auto'

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

class PastDataSetting {
    /* Used to capture options needed to create a plot of past results */

  public:
    PastDataSetting() = default;
    PastDataSetting(std::string s, int d_min, int d_max, int n_results)
        : what{std::move(s)}, dur_min{d_min}, dur_max{d_max}, num_of_results{n_results} {}

    bool operator==(const PastDataSetting&) const = default;

    bool operator!=(const PastDataSetting& other) const { return !(*this == other); }

    [[nodiscard]] auto get_what() const { return what; }

    [[nodiscard]] auto get_dur_minmax() const { return std::make_tuple(dur_min, dur_max); }

    [[nodiscard]] auto get_num_of_results() const { return num_of_results; }

  private:
    std::string what;
    int dur_min{0};
    int dur_max{0};
    int num_of_results{0};
};

inline auto create_select_query(const TabScore& tab, const PastDataSetting& ps) {
    auto what = ps.get_what();
    auto [dur_min, dur_max] = ps.get_dur_minmax();
    auto num_of_results = static_cast<unsigned long>(ps.get_num_of_results());
    return select(all_of(tab))
        .from(tab)
        .order_by(tab.id.desc())
        .where(tab.test_duration >= dur_min and tab.test_duration <= dur_max)
        .limit(num_of_results);
}

inline auto get_result_from_db(const PastDataSetting& ps) {
    using TimePoint = decltype(std::chrono::system_clock::now());

    const auto tab = TabScore{};
    auto db = get_db_connection();
    std::vector<float> db_values{};
    std::vector<TimePoint> db_timepoints{};
    for (const auto& row : db(create_select_query(tab, ps))) {
        float what{};
        if (ps.get_what() == "WPM") {
            what = static_cast<float>(row.wpm);
        } else if (ps.get_what() == "CPM") {
            what = static_cast<float>(row.cpm);
        } else if (ps.get_what() == "words_correct") {
            what = static_cast<float>(row.words_correct);
        } else if (ps.get_what() == "words_bad") {
            what = static_cast<float>(row.words_bad);
        } else if (ps.get_what() == "backspaces") {
            what = static_cast<float>(row.backspaces);
        } else {
            // this shouldn't happen
            what = -1.0F;
        }
        auto dtime = row.test_datetime.value();
        db_values.push_back(what);
        db_timepoints.emplace_back(dtime);
    }
    // query has order_by(tab.id.desc()) to pick n last results (chronologically)
    // we reverse it so it's in chronological order for plotting
    // TODO: maybe you can write function to order by the datetime
    std::reverse(db_values.begin(), db_values.end());
    std::reverse(db_timepoints.begin(), db_timepoints.end());
    return std::make_pair(std::move(db_values), std::move(db_timepoints));
}

class PastData {

  public:
    using TimePoint = decltype(std::chrono::system_clock::now());

    auto get_past_timepoints() {
        renew_from_database(setting); // we need fresh timepoints, setting reused
        return past_timepoints;
    }
    auto get_past_data(const PastDataSetting& s) {
        renew_from_database(s);
        return past_data;
    }

    void invalidate() { is_valid = false; }

  private:

    void renew_from_database(const PastDataSetting& s) {
        if (is_valid && s == setting) {
            return;
        }
        setting = s;
        is_valid = true;
        std::tie(past_data, past_timepoints) = get_result_from_db(s);
    }

    PastDataSetting setting;
    std::vector<float> past_data;
    std::vector<TimePoint> past_timepoints;
    bool is_valid;
};

} // namespace speedtyper
#endif /* ifndef SPEEDTYPER_DATABASE_UTIL_FUNCTIONS */
