#include <iostream>
#include <stdexcept>
/* #include <imgui.h> */
/* #include <gsl/gsl-lite.hpp> */

int main(){
    std::cout << "hello\n";
    return 0;
}


/* int main(){ */
/*     try { */
/*         std::cout << "ImGUI program starts...\n"; */
/*         IMGUI_CHECKVERSION(); */
/*         ImGui::CreateContext(); */
/*         ImGuiIO& io = ImGui::GetIO(); */
/*         ImGui::Text("Hello, world %d", 123); */
/*         if (ImGui::Button("Save")) */
/*             std::cout << "Save button pressed\n"; */
/*         char buf[128]; */
/*         ImGui::InputText("string", buf, IM_ARRAYSIZE(buf)); */
/*         float f = 0.0; */
/*         ImGui::SliderFloat("float", &f, 0.0f, 1.0f); */
/*         return 0; */
/*     } */
/*     catch(std::exception& e){ */
/*         std::cerr << e.what() << "\n"; */
/*         return 1; */
/*     } */
/*     catch(...){ */
/*         std::cerr << "something went wrong\n"; */
/*         return 2; */
/*     } */
/* } */

/* int main(int, char**) */
/* { */
/*     // main null, doesn't draw anything, used to test compilation */
/*     IMGUI_CHECKVERSION(); */
/*     ImGui::CreateContext(); */
/*     ImGuiIO& io = ImGui::GetIO(); */

/*     // Build atlas */
/*     unsigned char* tex_pixels = NULL; */
/*     int tex_w, tex_h; */
/*     io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h); */

/*     for (int n = 0; n < 20; n++) */
/*     { */
/*         printf("NewFrame() %d\n", n); */
/*         io.DisplaySize = ImVec2(1920, 1080); */
/*         io.DeltaTime = 1.0f / 60.0f; */
/*         ImGui::NewFrame(); */

/*         static float f = 0.0f; */
/*         ImGui::Text("Hello, world!"); */
/*         ImGui::SliderFloat("float", &f, 0.0f, 1.0f); */
/*         ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate); */
/*         ImGui::ShowDemoWindow(NULL); */

/*         ImGui::Render(); */
/*     } */

/*     printf("DestroyContext()\n"); */
/*     ImGui::DestroyContext(); */
/*     return 0; */
/* } */


/* #include "imgui.h" */
/* #include "imgui-SFML.h" */

/* #include <SFML/Graphics/RenderWindow.hpp> */
/* #include <SFML/System/Clock.hpp> */
/* #include <SFML/Window/Event.hpp> */
/* #include <SFML/Graphics/CircleShape.hpp> */

/* int main() */
/* { */
/*     sf::RenderWindow window(sf::VideoMode(640, 480), "ImGui + SFML = <3"); */
/*     window.setFramerateLimit(60); */
/*     ImGui::SFML::Init(window); */

/*     sf::CircleShape shape(100.f); */
/*     shape.setFillColor(sf::Color::Green); */

/*     sf::Clock deltaClock; */
/*     while (window.isOpen()) { */
/*         sf::Event event; */
/*         while (window.pollEvent(event)) { */
/*             ImGui::SFML::ProcessEvent(event); */

/*             if (event.type == sf::Event::Closed) { */
/*                 window.close(); */
/*             } */
/*         } */

/*         ImGui::SFML::Update(window, deltaClock.restart()); */

/*         ImGui::Begin("Hello, world!"); */
/*         ImGui::Button("Look at this pretty button"); */
/*         ImGui::End(); */

/*         window.clear(); */
/*         window.draw(shape); */
/*         ImGui::SFML::Render(window); */
/*         window.display(); */
/*     } */

/*     ImGui::SFML::Shutdown(); */
/* } */
