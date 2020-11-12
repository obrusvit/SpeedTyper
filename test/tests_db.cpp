#include <catch2/catch.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>

#include "TabSample.h"
#include <sqlpp11/custom_query.h>
#include <sqlpp11/sqlite3/sqlite3.h>
#include <sqlpp11/sqlpp11.h>

#ifdef SQLPP_USE_SQLCIPHER
#include <sqlcipher/sqlite3.h>
#else
#include <sqlite3.h>
#endif

SQLPP_ALIAS_PROVIDER(left)
/* SQLPP_ALIAS_PROVIDER(pragma) */
/* SQLPP_ALIAS_PROVIDER(sub) */

TEST_CASE("Database") {
    namespace sql = sqlpp::sqlite3;
    SECTION("Setup the database") {
        REQUIRE(1 == 1);
        sql::connection_config config;
        config.path_to_database = ":memory:";
        config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
        config.debug = true;
        sql::connection db(config);
        db.execute(R"(CREATE TABLE tab_sample (
                    alpha INTEGER PRIMARY KEY,
                    beta varchar(255) DEFAULT NULL,
                    gamma bool DEFAULT NULL))");
        db.execute(R"(CREATE TABLE tab_foo (
                    omega bigint(20) DEFAULT NULL))");

        const auto tab = TabSample{};

        // clear the table
        db(remove_from(tab).unconditionally());
        // explicit all_of(tab)
        for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally())) {
            std::cout << "row.alpha: " << row.alpha << ", row.beta: " << row.beta
                      << ", row.gamma: " << row.gamma << std::endl;
        };
        std::cerr << __FILE__ << ": " << __LINE__ << std::endl;
        // selecting a table implicitly expands to all_of(tab)
        for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally())) {
            std::cerr << "row.alpha: " << row.alpha << ", row.beta: " << row.beta
                      << ", row.gamma: " << row.gamma << std::endl;
        };

        // selecting thwo multicolumns
        for (const auto& row : db(select(multi_column(tab.alpha, tab.beta, tab.gamma).as(left),
                                         multi_column(all_of(tab)).as(tab))
                                      .from(tab)
                                      .unconditionally())) {
            std::cerr << "row.left.alpha: " << row.left.alpha
                      << ", row.left.beta: " << row.left.beta
                      << ", row.left.gamma: " << row.left.gamma << std::endl;
            std::cerr << "row.tabSample.alpha: " << row.tabSample.alpha
                      << ", row.tabSample.beta: " << row.tabSample.beta
                      << ", row.tabSample.gamma: " << row.tabSample.gamma << std::endl;
        }
        // insert
        std::cerr << "no of required columns: " << TabSample::_required_insert_columns::size::value
                  << std::endl;
        db(insert_into(tab).default_values());
        std::cout << "Last Insert ID: " << db.last_insert_id() << "\n";
        db(insert_into(tab).set(tab.gamma = true));
        std::cout << "Last Insert ID: " << db.last_insert_id() << "\n";
        auto di = dynamic_insert_into(db, tab).dynamic_set(tab.gamma = true);
        di.insert_list.add(tab.beta = "some_val");
        db(di);

        // explicit all_of(tab)
        for (const auto& row : db(select(all_of(tab)).from(tab).unconditionally())) {
            std::cout << "row.alpha: " << row.alpha << ", row.beta: " << row.beta
                      << ", row.gamma: " << row.gamma << std::endl;
        };
    }
}
