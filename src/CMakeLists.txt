add_library(words_provider
    WordsProvider.cpp
    WordsProvider.h)
target_link_libraries(words_provider
    PRIVATE project_options project_warnings
    fmt::fmt)

add_library(score
    Score.cpp
    Score.h)
target_link_libraries(score
    PRIVATE project_options project_warnings
    fmt::fmt)
target_include_directories(score
    PRIVATE ../include)

add_library(database_score
    DatabaseScore.cpp
    DatabaseScore.h)
target_link_libraries(database_score
    PRIVATE project_options project_warnings
    fmt::fmt
    PUBLIC
    sqlpp11
    # sqlpp11::sqlpp11-connector-sqlite3)
    sqlite3)
target_include_directories(database_score
    PRIVATE ../include)

add_library(sfml_components
    SfmlComponents.cpp
    SfmlComponents.h)
target_link_libraries(sfml_components
    PRIVATE project_options project_warnings
        words_provider
        sfml-system sfml-window sfml-network sfml-graphics sfml-audio
        fmt::fmt)
target_include_directories(sfml_components
    PRIVATE ../include)

# main executable
find_package(X11 REQUIRED)
add_executable(speed_typer main.cpp)
target_link_libraries(speed_typer
    PRIVATE project_options project_warnings
        score
        database_score
        sfml_components
        matplot
        sfml-system sfml-window sfml-network sfml-graphics sfml-audio
        ImGui-SFML::ImGui-SFML
        fmt::fmt
        ${X11_LIBRARIES})
target_include_directories(speed_typer
    PRIVATE ../include)
