# automatically enable catch2 to generate ctest targets
if(CONAN_CATCH2_ROOT_DEBUG)
  include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
  include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)

add_executable(tests tests.cpp)
target_link_libraries(tests
    PRIVATE
    project_options project_warnings
    words_provider
    sfml_components
    catch_main CONAN_PKG::fmt)

add_executable(tests_db tests_db.cpp)
target_link_libraries(tests_db
    PRIVATE
    project_options project_warnings
    catch_main
    CONAN_PKG::fmt
    CONAN_PKG::sqlpp11
    CONAN_PKG::sqlpp11-connector-sqlite3)
target_include_directories(tests_db
    PRIVATE
    ../include
    ../src)

add_executable(test_gui test_gui.cpp)
target_link_libraries(test_gui
    PRIVATE
    project_options project_warnings
    sfml_components
    # implot
    CONAN_PKG::sfml CONAN_PKG::imgui-sfml CONAN_PKG::fmt)
target_include_directories(test_gui
    PRIVATE
    ../include
    ../src
    ../src/implot)

add_executable(test_imgui_demo imgui_demo.cpp test_imgui_demo.cpp)
target_link_libraries(test_imgui_demo
    PRIVATE
    # project_options project_warnings
    CONAN_PKG::imgui-sfml)


# automatically discover tests that are defined in catch based test files you can modify the unittests. TEST_PREFIX to
# whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  EXTRA_ARGS
  -s
  --reporter=xml
  --out=tests.xml)

