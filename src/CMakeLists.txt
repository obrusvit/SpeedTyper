add_library(words_provider
    WordsProvider.cpp
    WordsProvider.h)
target_link_libraries(words_provider
    PRIVATE project_options project_warnings CONAN_PKG::fmt)

add_library(word_entity
    WordEntity.cpp
    WordEntity.h)
target_link_libraries(word_entity
    PRIVATE project_options project_warnings CONAN_PKG::gsl-lite CONAN_PKG::sfml)

add_library(displayed_words
    DisplayedWords.cpp
    DisplayedWords.h)
target_link_libraries(displayed_words
    PRIVATE word_entity words_provider project_options project_warnings CONAN_PKG::sfml)
target_include_directories(displayed_words
    PRIVATE ../include)

# main executable
add_executable(speed_typer main.cpp)
target_link_libraries(speed_typer
    PRIVATE project_options project_warnings
        displayed_words
        CONAN_PKG::fmt CONAN_PKG::gsl-lite)
target_include_directories(speed_typer
    PRIVATE ../include)
