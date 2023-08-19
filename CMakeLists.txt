cmake_minimum_required(VERSION 3.16)
project(SpeedTyper VERSION 1.0)

include(cmake/StandardProjectSettings.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# CPM dependencies
include(cmake/CPM.cmake)

# imgui-sfml
CPMAddPackage("gh:SFML/SFML#2.6.0")
CPMAddPackage("gh:ocornut/imgui#docking")
CPMAddPackage(
    GITHUB_REPOSITORY "eliasdaler/imgui-sfml"
    VERSION "2.6"
    OPTIONS "IMGUI_DIR ${imgui_SOURCE_DIR}"
)
add_compile_definitions(IMGUI_DISABLE_DEMO_WINDOWS) # imgui-sfml doesn't compile demo by default

# sqlpp11
CPMAddPackage("gh:rbock/sqlpp11#0.63")

# catch2
CPMAddPackage("gh:catchorg/Catch2@3.4.0")

# fmt
CPMAddPackage("gh:fmtlib/fmt#10.1.0")

# matplotpp
CPMAddPackage(
    NAME matplotplusplus
    GITHUB_REPOSITORY alandefreitas/matplotplusplus
    GIT_TAG origin/master # or whatever tag you want
)

add_subdirectory(src)

option(ENABLE_TESTING "Enable Test Builds" ON)
if(ENABLE_TESTING)
  enable_testing()
  message("Building Tests.")
  add_subdirectory(test)
endif()
