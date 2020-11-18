#include "DatabaseScore.h"
#include <fmt/color.h>
#include <fmt/core.h>

namespace sql = sqlpp::sqlite3;

namespace speedtyper {

sql::connection get_db_connection(){
    // prepare connection
    sql::connection_config config;
    config.flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    config.debug = false;
    config.path_to_database = "../db/score_db.sl3";
    sql::connection db{config};
    return db;
}

void save_results_to_db(const speedtyper::Score& score){
    auto db = get_db_connection();
    const auto tab = TabScore{};
    auto query = create_insert_score_query(tab, score);
    db(query);
}

}  // namespace speedtyper
