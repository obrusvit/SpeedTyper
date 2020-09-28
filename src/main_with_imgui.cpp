#include <iostream>
#include <sstream>
#include <stdexcept>
#include <imgui.h>
#include <imgui-SFML.h>
#include <gsl/gsl-lite.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>


int main()
{
    int X = 640;
    int Y = 480;
    sf::RenderWindow window(sf::VideoMode(640, 480), "ImGui + SFML = <3");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);


    sf::Font font;
    if ( ! font.loadFromFile("../assets/ubuntu-font-family-0.83/ubuntu-font-family-0.83/Ubuntu-L.ttf") ){
        std::cout << "font loading failed\n"; 
        return 1;
    }

    sf::Text text{"Hello world", font, 30};
    text.setFillColor(sf::Color::Red);
    /* text.setStyle(sf::Text::Bold | sf::Text::Underlined); */

    sf::Text input_display{"", font, 30};
    input_display.setPosition(0, Y/2);

    std::ostringstream oss{};

    // ImGui text input buffer
    int N = 64;
    char buf[N];

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.mouseButton.button == sf::Mouse::Right)
            {
                std::cout << "the right button was pressed" << std::endl;
                std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                std::cout << "mouse y: " << event.mouseButton.y << std::endl;
            }
            if (event.type == sf::Event::MouseEntered){
                std::cout << "the mouse cursor has entered the window" << std::endl;
                text.setString("Mouse cursor has entered the window");
            }

            if (event.type == sf::Event::MouseLeft){
                std::cout << "the mouse cursor has left the window" << std::endl;
                text.setString("Mouse cursor has left the window");
            }
            if (event.type == sf::Event::TextEntered){
                if (event.text.unicode < 128){
                    auto entered = static_cast<char>(event.text.unicode);
                    std::cout << "ASCII character typed: " << entered << std::endl;
                    if (entered == ' '){
                        oss.str("");
                        oss.clear();
                        input_display.setString("");
                    } else{
                        oss << entered;
                        input_display.setString(oss.str());
                    }
                }
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ImGui::Begin("Input");
        ImGui::InputText("Input text field", buf, N);
        ImGui::End();


        window.clear();
        window.draw(text);
        window.draw(input_display);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}
