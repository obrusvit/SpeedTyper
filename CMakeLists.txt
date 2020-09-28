cmake_minimum_required(VERSION 3.16)
project(SpeedTyper VERSION 1.0)

add_definitions("-std=c++2a")

# conan
include(cmake/Conan.cmake)
run_conan()
# conan_basic_setup()

include_directories(include)


option(ENABLE_TESTING "Enable Test Builds" ON)
if(ENABLE_TESTING)
  enable_testing()
  message("Building Tests.")
  add_subdirectory(test)
endif()

add_subdirectory(src)

# create compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
