add_executable(tests tests.cpp)
target_link_libraries(tests
    PRIVATE
    project_options project_warnings
    words_provider
    sfml_components
    sfml-system sfml-window sfml-network sfml-graphics sfml-audio
    Catch2::Catch2WithMain
    fmt::fmt)

add_executable(tests_db tests_db.cpp)
target_link_libraries(tests_db
    PRIVATE
    project_options project_warnings
    Catch2::Catch2WithMain
    fmt::fmt
    sqlpp11
    sqlite3)
target_include_directories(tests_db
    PRIVATE
    ../include
    ../src)

add_executable(test_gui test_gui.cpp)
target_link_libraries(test_gui
    PRIVATE
    project_options project_warnings
    sfml_components
    sfml-system sfml-window sfml-network sfml-graphics sfml-audio
    ImGui-SFML::ImGui-SFML
    fmt::fmt)
target_include_directories(test_gui
    PRIVATE
    ../include
    ../src
    ../src/implot)

add_executable(test_imgui_demo imgui_demo.cpp test_imgui_demo.cpp)
target_link_libraries(test_imgui_demo
    PRIVATE
    # project_options project_warnings
    ImGui-SFML::ImGui-SFML
    )

add_executable(test_matplot test_matplot.cpp)
target_link_libraries(test_matplot
    PRIVATE
    project_options project_warnings
    Catch2::Catch2WithMain
    matplot)
