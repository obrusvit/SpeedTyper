cmake_minimum_required(VERSION 3.16)
project(SpeedTyper VERSION 1.0)

include(cmake/StandardProjectSettings.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_compile_options(project_options INTERFACE -fconcepts)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# conan
include(cmake/Conan.cmake)
run_conan()
# conan_basic_setup()

find_package(Matplot++ REQUIRED HINTS $ENV{MATPLOTPP_DIR})

add_subdirectory(src)

option(ENABLE_TESTING "Enable Test Builds" ON)
if(ENABLE_TESTING)
  enable_testing()
  message("Building Tests.")
  add_subdirectory(test)
endif()
