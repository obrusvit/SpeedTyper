add_library(words_provider words_provider.cpp)
target_link_libraries(words_provider PRIVATE CONAN_PKG::fmt)

# main executable
add_executable(speed_typer main.cpp)
target_link_libraries(speed_typer PRIVATE words_provider CONAN_PKG::fmt CONAN_PKG::sfml)
