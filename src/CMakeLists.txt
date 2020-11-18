add_library(words_provider
    WordsProvider.cpp
    WordsProvider.h)
target_link_libraries(words_provider
    PRIVATE project_options project_warnings
    CONAN_PKG::fmt)

add_library(score
    Score.cpp
    Score.h)
target_link_libraries(score
    PRIVATE project_options project_warnings
    CONAN_PKG::fmt)
target_include_directories(score
    PRIVATE ../include)

add_library(database_score
    DatabaseScore.cpp
    DatabaseScore.h)
target_link_libraries(database_score
    PRIVATE project_options project_warnings
    CONAN_PKG::fmt
    PUBLIC
    CONAN_PKG::sqlpp11
    CONAN_PKG::sqlpp11-connector-sqlite3)
target_include_directories(database_score
    PRIVATE ../include)

add_library(sfml_components
    SfmlComponents.cpp
    SfmlComponents.h)
target_link_libraries(sfml_components
    PRIVATE project_options project_warnings
        words_provider
        CONAN_PKG::gsl-lite
        CONAN_PKG::sfml)
target_include_directories(sfml_components
    PRIVATE ../include)

# main executable
add_executable(speed_typer main.cpp)
target_link_libraries(speed_typer
    PRIVATE project_options project_warnings
        score
        database_score
        sfml_components
        CONAN_PKG::imgui-sfml
        CONAN_PKG::fmt
        CONAN_PKG::gsl-lite)
target_include_directories(speed_typer
    PRIVATE ../include)
