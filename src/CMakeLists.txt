add_library(words_provider
    WordsProvider.cpp
    WordsProvider.h)
target_link_libraries(words_provider
    PRIVATE project_options project_warnings CONAN_PKG::fmt)

add_library(sfml_components
    SfmlComponents.cpp
    SfmlComponents.h)
target_link_libraries(sfml_components
    PRIVATE project_options project_warnings
        words_provider
        CONAN_PKG::gsl-lite CONAN_PKG::sfml)
target_include_directories(sfml_components
    PRIVATE ../include)

# main executable
add_executable(speed_typer main.cpp)
target_link_libraries(speed_typer
    PRIVATE project_options project_warnings
        sfml_components
        CONAN_PKG::fmt CONAN_PKG::gsl-lite)
target_include_directories(speed_typer
    PRIVATE ../include)
