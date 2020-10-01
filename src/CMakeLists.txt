add_library(words_provider
    words_provider.cpp
    ../include/words_provider.hpp
)
target_link_libraries(words_provider
    PRIVATE project_options project_warnings CONAN_PKG::fmt)
target_include_directories(words_provider PUBLIC ../include)

# main executable
add_executable(speed_typer main.cpp)
target_link_libraries(speed_typer
    PRIVATE project_options project_warnings
        words_provider
        CONAN_PKG::fmt CONAN_PKG::sfml CONAN_PKG::gsl-lite)
