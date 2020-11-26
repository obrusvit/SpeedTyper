#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "constants.h"
#include <SFML/Graphics/Font.hpp>
#include <iostream>
#include <array>
#include <fmt/core.h>
#include <imgui.h>
#include <imgui-SFML.h>
/* #include <implot.h> */


#include "SfmlComponents.h"

void setup_ImGui(sf::RenderWindow& window){
    ImGui::SFML::Init(window, false);
    auto IO = ImGui::GetIO();
    IO.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
    IO.Fonts->AddFontFromFileTTF("../assets/dejavu-sans/DejaVuSans.ttf", 12.f);
    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture
}
void show_past_results_plotting(){
    static int dur_min = 10;
    static int dur_max = 100;
    static bool is_span = false;
    ImGui::Begin("Past results"); 
    if (is_span){
        ImGui::DragInt("Duration min", &dur_min, 1.0f, 10, dur_max);
        ImGui::SameLine(); ImGui::Checkbox("Span", &is_span);
        ImGui::DragInt("Duration max", &dur_max, 1.0f, dur_min, 1200);
    } else {
        ImGui::DragInt("Duration", &dur_min, 1.0f, 10, 1200);
        ImGui::SameLine(); ImGui::Checkbox("Span", &is_span);
    }
    const char* items[] = { "WPM", "CPM", "Correct words", "Wrong words", "Backspaces"};
    static int item_current = 0;
    ImGui::SameLine(); ImGui::Combo("combo", &item_current, items, IM_ARRAYSIZE(items));
    /* ImGui::SameLine(); HelpMarker("Refer to the \"Combo\" section below for an explanation of the full BeginCombo/EndCombo API, and demonstration of various flags.\n"); */

    /* static float arr[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f }; */
    /* ImGui::PlotLines("Frame Times", arr, IM_ARRAYSIZE(arr)); */
    /* std::array<float, 7> arr{ 0.25f, 0.1f, 1.0f, 12.5f, 0.92f, 0.1f, 0.2f }; */
    std::array<float, 7> arr{ 5.25f, 5.1f, 5.0f, 5.5f, 5.92f, 5.1f, 5.2f };
    ImGui::PlotLines("Frame Times", arr.data(), arr.size());
    static int values_offset = 0;
    static const char *overlay = items[item_current];
    ImGui::PlotLines("Lines", arr.data(), arr.size(), values_offset, overlay, 5.0f, 6.0f, ImVec2(0,120));
    ImGui::End();
}

void test_implot(){
    // TODO try to integrate implot
    int   bar_data[5] = {1, 2, 2, 5, 1};

    ImGui::Begin("My Window");
    /* if (ImPlot::BeginPlot("My Plot")) { */
    /*     ImPlot::PlotBars("My Bar Plot", bar_data, 11); */
    /*     ImPlot::EndPlot(); */
    /* } */
    ImGui::End();

}

int main() {
    sf::RenderWindow window(
        sf::VideoMode(speedtyper::GUI_options::win_sz_X, speedtyper::GUI_options::win_sz_Y),
        "Test GUI");
    window.setFramerateLimit(speedtyper::GUI_options::FPS_LIMIT);

    setup_ImGui(window);

    sf::Font font;
    font.loadFromFile("../assets/dejavu-sans/DejaVuSans.ttf");

    speedtyper::Button button1({10, 10}, "Button1", font, [](){fmt::print("Button1 clicked\n");});

    int time_of_test = speedtyper::gameopt::seconds_limit_default;

    // run the program as long as the window is open
    sf::Clock deltaClock;
    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event{};
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (button1.hover(sf::Mouse::getPosition(window))) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    button1();
                }
            }
        }
        ImGui::SFML::Update(window, deltaClock.restart());
        ImGui::Begin("Time of a test");
        ImGui::Text("Just a text");
        ImGui::SetWindowPos({100, 100});
        ImGui::End();

        ImGui::Begin("Setting");
        ImGui::SetWindowPos({100, 200});
        ImGui::DragInt("Test time", &time_of_test, 1.0f, 10, 1200);
        ImGui::End();

        show_past_results_plotting();
        test_implot();


        window.clear();
        window.draw(button1);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();

    return 0;
}
