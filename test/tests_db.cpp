#include <catch2/catch.hpp>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <date/date.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <vector>

#include "Score.h"
#include "DatabaseScore.h"
#include "TabScore.h"
#include <sqlpp11/custom_query.h>
#include <sqlpp11/select_flags.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif

namespace sql = sqlpp::sqlite3;

SQLPP_ALIAS_PROVIDER(left)
/* SQLPP_ALIAS_PROVIDER(pragma) */
/* SQLPP_ALIAS_PROVIDER(sub) */

auto get_test_database_up_and_running(bool in_file = false) {
    sql::connection_config config;
    config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    config.debug = false;
    if (in_file) {
        config.path_to_database = "../test/tabscore_test_db.sl3";
    } else {
        config.path_to_database = ":memory:";
    }

    sql::connection db{config};

    if (in_file) {
        db.execute(R"(DROP TABLE IF EXISTS "tab_score";)");
    }

    db.execute(R"(CREATE TABLE "tab_score" (
            "id"	        INTEGER PRIMARY KEY AUTOINCREMENT UNIQUE,
            "words_correct"	INTEGER NOT NULL,
            "words_bad"	    INTEGER NOT NULL,
            "chars_correct"	INTEGER NOT NULL,
            "chars_bad"	    INTEGER NOT NULL,
            "backspaces"	INTEGER DEFAULT -1,  
            "spaces"	    INTEGER DEFAULT -1,
            "key_presses"	INTEGER DEFAULT -1,
            "wpm"           INTEGER NOT NULL,
            "cpm"           INTEGER DEFAULT -1,
            "test_duration"	INTEGER NOT NULL,
            "test_datetime" TEXT NOT NULL);)");
    // Nullable types are thos which are not present in 10mFF results
    return db;
}


TEST_CASE("Database") {
    using namespace speedtyper;

    SECTION("Score database basics") {
        auto db = get_test_database_up_and_running(false);
        const auto tab = TabScore{};

        /* auto result_1 = db(select(all_of(tab))); */
        auto result_1 = db(select(all_of(tab)).from(tab).unconditionally());
        REQUIRE(result_1.empty());
        REQUIRE(result_1.begin() == result_1.end());

        // insert and test size
        auto some_date =
            date::sys_days{date::year_month_day{date::year{2020}, date::month{8}, date::day{10}}};

        db(insert_into(tab).set(tab.words_correct = 50, tab.words_bad = 2, tab.chars_correct = 250,
                                tab.chars_bad = 10, tab.backspaces = 2, tab.spaces = 50,
                                tab.key_presses = 300, tab.wpm = 50, tab.cpm = 350,
                                tab.test_duration = 60, tab.test_datetime = some_date));
        auto result_2 = db(select(all_of(tab)).from(tab).unconditionally());
        REQUIRE_FALSE(result_2.empty());
        REQUIRE(result_2.begin() != result_2.end());
        auto d2 = std::distance(result_2.begin(), result_2.end());
        REQUIRE(d2 == 1);
        int i2 = 0;
        for ([[maybe_unused]] const auto& row : result_2) {
            ++i2;
        }
        REQUIRE(i2 == 0); // BE CAREFUL, std::distance invalidates result_t object
        /* print_rows_tabscore(db, tab); */

        // insert one more and test size
        const auto now = std::chrono::system_clock::now();
        db(insert_into(tab).set(tab.words_correct = 60, tab.words_bad = 0, tab.chars_correct = 360,
                                tab.chars_bad = 0, tab.backspaces = 0, tab.spaces = 60,
                                tab.key_presses = 360, tab.wpm = 60, tab.cpm = 200,
                                tab.test_duration = 60, tab.test_datetime = now));
        auto result_3 = db(select(all_of(tab)).from(tab).unconditionally());
        auto d3 = std::distance(result_3.begin(), result_3.end());
        REQUIRE(d3 == 2);
        /* print_rows_tabscore(db, tab); */

        // insert with different time
        db(insert_into(tab).set(tab.words_correct = 60, tab.words_bad = 0, tab.chars_correct = 360,
                                tab.chars_bad = 0, tab.backspaces = 0, tab.spaces = 60,
                                tab.key_presses = 360, tab.wpm = 60, tab.test_duration = 61,
                                tab.test_datetime = now));
        auto result_4 = db(select(all_of(tab)).from(tab).where(tab.test_duration == 61));
        auto result_5 = db(select(all_of(tab)).from(tab).where(tab.test_duration == 60));
        auto d4 = std::distance(result_4.begin(), result_4.end());
        auto d5 = std::distance(result_5.begin(), result_5.end());
        REQUIRE(d4 == 1);
        REQUIRE(d5 == 2);
        /* print_rows_tabscore(db, tab); */

        // remove one with id 3 (last one)
        db(remove_from(tab).where(tab.id == 3));
        auto result_6 = db(select(all_of(tab)).from(tab).unconditionally());
        auto d6 = std::distance(result_6.begin(), result_6.end());
        REQUIRE(d6 == 2);
        /* print_rows_tabscore(db, tab); */

        // get all words_correct to vector
        std::vector<long int> vec_words_correct{};
        auto result_7 =
            db(select(tab.test_duration, tab.words_correct).from(tab).unconditionally());
        for (const auto& row : result_7) {
            vec_words_correct.push_back(row.words_correct);
        }
        REQUIRE(vec_words_correct == std::vector<long int>{50, 60});
    }

    SECTION("TabScore database from score objects") {
        auto db = get_test_database_up_and_running(false);
        const auto tab = TabScore{};
        REQUIRE(db(select(all_of(tab)).from(tab).unconditionally()).empty());

        Score s1{120};
        s1.words_correct = 120;
        s1.words_bad = 2;
        s1.chars_correct = 600;
        s1.chars_bad = 10;
        s1.backspaces = 2;
        s1.spaces = 60;
        s1.key_presses = 700;

        auto insert_score_query = create_insert_score_query(tab, s1);
        db(insert_score_query);
        /* print_rows_tabscore(db, tab); */

        auto result_1 = db(select(all_of(tab)).from(tab).unconditionally());
        const auto& only_entry = result_1.front();
        REQUIRE(static_cast<int>(only_entry.words_correct) == 120);

        Score s2 = row_to_score(only_entry);
        REQUIRE(s1 == s2);
    }

    SECTION("TabScore database placed in real file") {
        auto db = get_test_database_up_and_running(true);
        const auto tab = TabScore{};
        REQUIRE(db(select(all_of(tab)).from(tab).unconditionally()).empty());

        Score s1{120};
        Score s2{60};
        Score s3{60};
        Score s4{60};

        db(create_insert_score_query(tab, s1));
        db(create_insert_score_query(tab, s2));
        db(create_insert_score_query(tab, s3));
        db(create_insert_score_query(tab, s4));

        auto result_1 = db(select(all_of(tab)).from(tab).where(tab.test_duration == 60));
        auto result_2 = db(select(all_of(tab)).from(tab).where(tab.test_duration == 120));
        REQUIRE(std::distance(result_1.begin(), result_1.end()) == 3);
        REQUIRE(std::distance(result_2.begin(), result_2.end()) == 1);

        auto result_3 = db(select(all_of(tab)).from(tab).where(tab.test_duration == 120));
        const auto& only_entry = result_3.front();
        REQUIRE(s1 == row_to_score(only_entry));
    }

    SECTION("TabScore database select n results"){
        auto db = get_test_database_up_and_running(false);
        const auto tab = TabScore{};
        REQUIRE(db(select(all_of(tab)).from(tab).unconditionally()).empty());

        Score s1{60};
        Score s2{61};
        Score s3{62};
        Score s4{63};

        db(create_insert_score_query(tab, s1));
        db(create_insert_score_query(tab, s2));
        db(create_insert_score_query(tab, s3));
        db(create_insert_score_query(tab, s4));

        unsigned long how_many_results = 2;
        auto result = db(select(all_of(tab)).from(tab).order_by(tab.id.desc()).unconditionally().limit(how_many_results));
        REQUIRE(result.front().id.value() == 4);
        REQUIRE(result.front().test_duration.value() == 63);

        auto dist = static_cast<unsigned long>(std::distance(result.begin(), result.end()));
        REQUIRE(dist == how_many_results);
    }
}

TEST_CASE("RealDatabase") {
    using namespace speedtyper;
    SECTION("TabScore with real database") {

        // prepare connection
        sql::connection_config config;
        config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        config.debug = false;
        config.path_to_database = "../db/score_db.sl3";
        sql::connection db{config};
        const auto tab = TabScore{};

        // get current size
        auto result_all = db(select(all_of(tab)).from(tab).unconditionally());
        auto db_size = std::distance(result_all.begin(), result_all.end());
        fmt::print("db_size, start of TabScore with real database test: {}\n", db_size);

        // insert something
        Score s1{42};
        s1.words_correct = 42;
        s1.words_bad = 42;
        s1.chars_correct = 42;
        s1.chars_bad = 42;
        s1.spaces = 42;
        db(create_insert_score_query(tab, s1));

        // check it's there
        auto result_1 = db(
            select(all_of(tab))
                .from(tab)
                .where(tab.test_duration == 42 && tab.words_correct == 42 && tab.words_bad == 42));
        REQUIRE(s1 == row_to_score(result_1.front()));

        // remove it
        db(remove_from(tab).where(tab.test_duration == 42 and tab.words_correct == 42 and tab.words_bad == 42));

        // check the db is the same as before
        auto result_all_final = db(select(all_of(tab)).from(tab).unconditionally());
        auto db_size_final = std::distance(result_all_final.begin(), result_all_final.end());
        fmt::print("db_size, end of TabScore with real database test: {}\n", db_size_final);
        REQUIRE(db_size == db_size_final);
    }
}
