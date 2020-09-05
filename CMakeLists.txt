cmake_minimum_required(VERSION 3.16)
project(SpeedTyper VERSION 1.0)

add_definitions("-std=c++2a")


# conan
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()
include(cmake/Conan.cmake)
run_conan()
conan_basic_setup()

include_directories(include)

# imgui test
add_executable(test_imgui test.cpp)
target_link_libraries(test_imgui PRIVATE CONAN_PKG::imgui-sfml)

# main executable
add_executable(speed_typer_main main.cpp)
# target_link_libraries(speed_typer_main PRIVATE CONAN_PKG::imgui-sfml)
target_link_libraries(speed_typer_main PRIVATE ${CONAN_LIBS})

# create compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
