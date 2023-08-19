#include <algorithm>
#include <atomic>
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <functional> // ref()
#include <iostream>
#include <matplot/axes_objects/box_chart.h>
#include <matplot/freestanding/axes_functions.h>
#include <matplot/freestanding/plot.h>
#include <matplot/util/handle_types.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

#include "SFML/Graphics/Drawable.hpp"
#include "SFML/Graphics/Shape.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <X11/Xlib.h> // XInitThreads() otherwise error with [xcb]
#include <imgui-SFML.h>
#include <imgui.h>
#include <matplot/matplot.h>

#include "DatabaseScore.h"
#include "SFML/Window/Mouse.hpp"
#include "SFML/Window/WindowStyle.hpp"
#include "Score.h"
#include "SfmlComponents.h"
#include "Timer.h"
#include "constants.h"

using namespace speedtyper;

void handle_backspace(Score& score, DisplayedWords& display, std::ostringstream& oss) {
    auto so_far = oss.str();
    if (so_far.empty()) {
        return;
    }
    so_far.pop_back();
    oss.str(so_far);
    oss.clear();
    oss.seekp(0, std::ios_base::end); // set writing to the end
    score.backspaces++;
    score.key_presses++;
    display.update(oss.str());
}

void handle_space(Score& score, DisplayedWords& display, std::ostringstream& oss) {
    score.add_word(oss.str(), display.get_current_word().get_string());
    score.spaces++;
    score.key_presses++;
    display.next_word(oss.str());
    oss.str("");
    oss.clear();
}

void handle_written_char(Score& score, DisplayedWords& display, std::ostringstream& oss,
                         unsigned int unicode) {
    if (unicode > ASCII_values::key_space && unicode < ASCII_values::limit) {
        auto entered = static_cast<char>(unicode);
        oss << entered;
        score.key_presses++;
        display.update(oss.str());
    }
}

void setup_ImGui(sf::RenderWindow& window) {
    ImGui::SFML::Init(window, false);
    auto IO = ImGui::GetIO();
    /* IO.Fonts */
    /*     ->Clear(); // clear fonts if you loaded some before (even if only default one was loaded)
     */
    /* IO.Fonts->AddFontFromFileTTF("../assets/dejavu-sans/DejaVuSans.ttf", 12.f); */
    ImGui::SFML::UpdateFontTexture(); // important call: updates font texture

    ImGuiStyle& style = ImGui::GetStyle();
    style.TabRounding = 0.0F;
    style.GrabRounding = 0.0F;
    style.FrameRounding = 0.0F;
    style.ChildRounding = 0.0F;
    style.PopupRounding = 0.0F;
    style.WindowRounding = 0.0F;
    style.ScrollbarRounding = 0.0F;
}

void show_settings(int* test_duration, bool* save_to_db, float y_pos) {
    ImGui::SetNextWindowPos({10, y_pos});
    ImGui::SetNextWindowSize({200, 250});
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::DragInt("test time", test_duration, 1.0F, gameopt::seconds_limit_min,
                   gameopt::seconds_limit_max);
    ImGui::Checkbox("Save to db", save_to_db);
    ImGui::End();
}

void plot_with_matplotpp_data(auto y_data, const std::string& y_label) {

    using namespace matplot;
    /* auto f1 = matplot::figure(true); */
    auto f = figure(true);
    /* auto ax = f->gca(); */
    auto ax = f->current_axes();
    auto p = ax->plot(y_data);
    p->color("blue").line_width(2);
    ax->ylabel(y_label);
    ax->xlabel("test nr [-]");
    f->draw();
}

auto get_xticks_from_timepoints(auto data_timepoints) {
    using namespace std::chrono;
    constexpr size_t n_of_ticks = 10;
    std::vector<double> tick_points{};
    std::vector<std::string> tick_labels{};
    auto first_timepoint = data_timepoints.front();
    if (data_timepoints.size() > n_of_ticks) {
        tick_labels.reserve(n_of_ticks);
        auto n = data_timepoints.size();
        auto step = n / n_of_ticks;
        for (auto iter = data_timepoints.begin(); iter < data_timepoints.end();
             std::advance(iter, step)) {
            tick_points.push_back(duration_cast<seconds>((*iter) - first_timepoint).count());
            tick_labels.push_back(fmt::format("{:%d.%m.%Y}", *iter));
        }
        if (tick_points.back() !=
            duration_cast<seconds>(data_timepoints.back() - first_timepoint).count()) {
            // include also the very last entry if the previous iteration didn't hit the most recent
            // point
            tick_points.push_back(
                duration_cast<seconds>(data_timepoints.back() - first_timepoint).count());
            tick_labels.push_back(fmt::format("{:%d.%m.%Y}", data_timepoints.back()));
        }
    } else {
        // if there are not that many, just put them all
        tick_labels.reserve(data_timepoints.size());
        for (const auto& timepoint : data_timepoints) {
            tick_points.push_back(duration_cast<seconds>(timepoint - first_timepoint).count());
            tick_labels.push_back(fmt::format("{:%d.%m.%Y}", timepoint));
        }
    }
    return std::make_tuple(tick_points, tick_labels);
}

std::vector<double> get_xdata_from_timepoints(auto data_timepoints) {
    using namespace std::chrono;
    std::vector<double> res{};
    res.reserve(data_timepoints.size());
    auto first_timepoint = data_timepoints.front();
    for (const auto& timepoint : data_timepoints) {
        res.push_back(duration_cast<seconds>(timepoint - first_timepoint).count());
    }
    return res;
}

void plot_with_matplotpp_data_timepoints(auto data_timepoints, auto y_data,
                                         const std::string& y_label) {
    using namespace matplot;
    if (data_timepoints.size() == 0) {
        ImGui::Text("No timepoints.");
        return;
    }
    if (data_timepoints.size() != y_data.size()) {
        ImGui::Text("Timepoints and data mismatch.");
        return;
    }
    auto x_data = get_xdata_from_timepoints(data_timepoints);
    auto [x_ticks, x_ticks_labels] = get_xticks_from_timepoints(data_timepoints);
    /* auto f1 = matplot::figure(true); */
    auto f = figure(true);
    /* auto ax = f->gca(); */
    auto ax = f->current_axes();
    auto p = ax->plot(x_data, y_data);
    p->color("blue").line_width(2);
    ax->ylabel(y_label);
    ax->xlabel("Timepoint");
    ax->xticks(x_ticks);
    ax->xticklabels(x_ticks_labels);
    ax->xtickangle(45);

    f->draw();
}

void show_past_results_plot(PastData& past_data, float y_pos) {
    static int n_results = 10;
    static std::array<int, 2> dur_min_max{10, 100};
    static const std::array<const char*, 5> items = {"WPM", "CPM", "words_correct", "words_bad",
                                                     "backspaces"};
    static int item_current = 0;
    static bool plot_with_timepoints = false;
    ImGui::SetNextWindowPos({240, y_pos});
    ImGui::SetNextWindowSize({500, 250});
    ImGui::Begin("Past results", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::DragInt("Number of results", &n_results, 1.0F, 0, 10'000);
    ImGui::DragIntRange2("Min/Max dur", &dur_min_max.at(0), &dur_min_max.at(1), 1.0F,
                         gameopt::seconds_limit_min, gameopt::seconds_limit_max);
    ImGui::SameLine();
    ImGui::SameLine();
    ImGui::Combo("combo", &item_current, items.data(), items.size());

    const char* overlay = items.at(static_cast<decltype(items)::size_type>(item_current));
    PastDataSetting setting{std::string{overlay}, dur_min_max.at(0), dur_min_max.at(1), n_results};
    auto data = past_data.get_past_data(setting);
    auto scale_max = std::max_element(data.begin(), data.end());
    auto scale_min = std::min_element(data.begin(), data.end());
    if (!data.empty()) {
        ImGui::PlotLines("Past results", data.data(), static_cast<int>(data.size()), 0, overlay,
                         *scale_min, *scale_max, ImVec2(0, 140));
        ImGui::Checkbox("With timepoints", &plot_with_timepoints);
        ImGui::SameLine();
        if (ImGui::Button("Make proper plot.")) {
            if (plot_with_timepoints) {
                auto data_timepoints = past_data.get_past_timepoints();
                plot_with_matplotpp_data_timepoints(data_timepoints, data, setting.get_what());
            } else {
                plot_with_matplotpp_data(data, setting.get_what());
            }
        }
    } else {
        ImGui::Text("No results with given settings");
    }
    ImGui::End();
}

void show_results_imgui(const Score& score, float y_pos) {
    constexpr auto report_width = 25;
    constexpr auto number_width = 6;
    constexpr auto description_width = report_width - number_width;

    ImGui::SetNextWindowPos({800, y_pos});
    ImGui::SetNextWindowSize({200, 250});
    ImGui::Begin("Current result:", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::Separator();
    ImGui::Text("%s", fmt::format("{0}\n", "Words").c_str());
    ImGui::TextColored(ImVec4(0.0F, 1.0F, 0.0F, 1.0F), "%s",
                       fmt::format("{0:.<{2}}{1:.>{3}}\n", "Correct:", score.words_correct,
                                   description_width, number_width)
                           .c_str());
    ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "%s",
                       fmt::format("{0:.<{2}}{1:.>{3}}\n", "Incorrect:", score.words_bad,
                                   description_width, number_width)
                           .c_str());
    auto chars_typed = fmt::format("{0}|{1}", score.chars_correct, score.chars_bad);

    ImGui::Separator();
    ImGui::Text("%s", fmt::format("{0}\n", "Keys").c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}}\n", "Chars typed:", chars_typed,
                                  description_width, number_width)
                          .c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}.3}\n", "Accuracy:", score.calculate_accuracy(),
                                  description_width, number_width)
                          .c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}}\n", "Total key presses:", score.key_presses,
                                  description_width, number_width)
                          .c_str());
    ImGui::TextColored(ImVec4(1.0F, 0.0F, 0.0F, 1.0F), "%s",
                       fmt::format("{0:.<{2}}{1:.>{3}}\n", "Backspace:", score.backspaces,
                                   description_width, number_width)
                           .c_str());

    ImGui::Separator();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", fmt::format("{0}\n", "Summary").c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}.2}{1:.>{3}}\n", "CPM:", score.calculate_cpm(),
                                  description_width, number_width)
                          .c_str());
    ImGui::Text("%s", fmt::format("{0:.<{2}}{1:.>{3}}\n", "WPM:", score.calculate_wpm(),
                                  description_width, number_width)
                          .c_str());
    ImGui::End();
}

enum class SpeedTyperStatus { waiting_for_start, running, finished, showing_results };

int main() {
    XInitThreads();
    sf::RenderWindow window(sf::VideoMode(GUI_options::win_sz_X, GUI_options::win_sz_Y), "Typer++",
                            sf::Style::Close); // Close is to turn off resize ability
    window.setFramerateLimit(GUI_options::FPS_LIMIT);
    setup_ImGui(window);

    sf::Font font;
    if (!font.loadFromFile("../assets/dejavu-sans/DejaVuSans.ttf")) {
        fmt::print(fg(fmt::color::red), "font loading failed\n");
        return 1;
    }

    //------------------------------------------------------------------------------
    // State data

    std::ostringstream oss{};
    std::atomic<SpeedTyperStatus> typer_status = SpeedTyperStatus::waiting_for_start;
    Timer timer;
    auto timer_current_task_id = 0;
    auto test_duration = gameopt::seconds_limit_default; // [s]
    auto save_to_db = true;
    PastData past_data;
    Score score{test_duration};
    Score past_score{};

    //------------------------------------------------------------------------------
    // GUI objects

    DisplayedWords displayed_words{font};
    InputField input_field{font};
    const auto imgui_stuff_y_pos = input_field.get_position().y + 100.0F;
    std::vector<std::unique_ptr<sf::Drawable>> owning_drawables;
    auto reset_button_pos_x = input_field.get_position().x + input_field.get_size().x + 5.0F;
    auto reset_button_pos_y = input_field.get_position().y;
    auto reset_button_pos = sf::Vector2f{reset_button_pos_x, reset_button_pos_y};
    Button reset_button(reset_button_pos, "Reset", font, [&]() {
        displayed_words.reset();
        input_field.reset();
        oss.str("");
        /* timer.disable(timer_current_task_id); */
        timer.disable_all();
        typer_status = SpeedTyperStatus::waiting_for_start;
        owning_drawables.clear();
    });
    auto timer_display_pos_x = reset_button_pos_x + reset_button.get_size().x + 5.0F;
    auto timer_display_pos_y = reset_button_pos_y;
    auto timer_display_pos = sf::Vector2f{timer_display_pos_x, timer_display_pos_y};
    TimerDisplay timer_display{timer_display_pos, test_duration, font};
    std::vector<sf::Drawable*> not_owning_drawables{&input_field, &displayed_words, &reset_button,
                                                    &timer_display};

    //------------------------------------------------------------------------------
    // Delegate rendering to another thread

    sf::Clock deltaClock;
    window.setActive(false);
    auto rendering = [&]() {
        window.setActive(true);
        while (window.isOpen()) {
            ImGui::SFML::Update(window, deltaClock.restart());
            show_settings(&test_duration, &save_to_db, imgui_stuff_y_pos);
            show_results_imgui(past_score, imgui_stuff_y_pos);
            show_past_results_plot(past_data, imgui_stuff_y_pos);
            window.clear();
            timer_display.set_time_s(
                timer.get_remaining_time_s(timer_current_task_id, test_duration));
            for (const auto& ptr_drawable : not_owning_drawables) {
                window.draw(*ptr_drawable);
            }
            for (const auto& ptr_drawable : owning_drawables) {
                window.draw(*ptr_drawable);
            }
            ImGui::SFML::Render(window);
            window.display();
        }
        ImGui::SFML::Shutdown();
    };
    std::thread render_thread{rendering};

    //------------------------------------------------------------------------------

    auto typing_loop_condition = [&typer_status, &input_field](const sf::Event& event) {
        return input_field.is_active() && event.type == sf::Event::TextEntered &&
               (typer_status == SpeedTyperStatus::running ||
                typer_status == SpeedTyperStatus::waiting_for_start);
    };

    auto func_timeout = [&typer_status]() { typer_status = SpeedTyperStatus::finished; };

    //------------------------------------------------------------------------------

    while (window.isOpen()) {
        sf::Event event{};
        while (window.waitEvent(event)) {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                /* Resize everything accordingly */
                /* window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, */
                /*                 event.size.height))); */
            }

            if (typer_status == SpeedTyperStatus::showing_results) {
                // intentionally empty for now
            }

            if (typer_status == SpeedTyperStatus::finished) {
                input_field.set_bg_color(sf::Color::Yellow);
                typer_status = SpeedTyperStatus::showing_results;
                past_score = score;
                show_results_terminal(past_score);
                if (save_to_db) {
                    save_results_to_db(past_score);
                    past_data.invalidate();
                }

                auto result = fmt::format("Done, WPM is {}", past_score.calculate_wpm());
                auto txt = std::make_unique<sf::Text>(result, font, GUI_options::gui_font_sz);
                txt->setPosition(input_field.get_position().x, input_field.get_position().y + 50);
                owning_drawables.push_back(std::move(txt));
            }

            // focus/unfocus the InputField
            if (typer_status.load() == SpeedTyperStatus::waiting_for_start) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (input_field.hover(sf::Mouse::getPosition(window))) {
                        input_field.set_active(true);
                    } else {
                        input_field.set_active(false);
                    }
                }
            }

            // toggl visibility of the TimerDisplay
            if (timer_display.hover(sf::Mouse::getPosition(window))) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    timer_display.toggl_visibility();
                    input_field.set_active(true);
                }
            }

            // handle buttons
            if (reset_button.hover(sf::Mouse::getPosition(window))) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    reset_button();
                }
            }

            if (typing_loop_condition(event)) {
                const auto unicode = event.text.unicode;
                switch (unicode) {
                case ASCII_values::key_backspace:
                    handle_backspace(score, displayed_words, oss);
                    break;
                case ASCII_values::key_space:
                    handle_space(score, displayed_words, oss);
                    break;
                default:
                    if (typer_status == SpeedTyperStatus::waiting_for_start) {
                        using namespace std::chrono;
                        auto timeout = duration_cast<milliseconds>(seconds(test_duration));
                        typer_status = SpeedTyperStatus::running;
                        score = Score{test_duration};
                        timer_current_task_id = timer.set_timeout(func_timeout, timeout);
                    }
                    handle_written_char(score, displayed_words, oss, unicode);
                    break;
                }
                input_field.set_string(oss.str());
            }
        }
    }
    render_thread.join();
    return 0;
}
