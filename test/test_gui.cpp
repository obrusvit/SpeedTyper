#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Window/Window.hpp"
#include "constants.h"
#include <SFML/Graphics/Font.hpp>
#include <iostream>
#include <fmt/core.h>
#include <imgui.h>
#include <imgui-SFML.h>

#include "SfmlComponents.h"

void setup_ImGui(sf::RenderWindow& window){
    ImGui::SFML::Init(window, false);
    auto IO = ImGui::GetIO();
    IO.Fonts->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
    IO.Fonts->AddFontFromFileTTF("../assets/dejavu-sans/DejaVuSans.ttf", 30.f);
    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture
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

    int time_of_test = speedtyper::gameopt::seconds_limit;

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


        window.clear();
        window.draw(button1);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();

    return 0;
}
