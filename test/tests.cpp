#include "../src/WordsProvider.h"
#include "../src/SfmlComponents.h"
#include "../src/Timer.h"
#include "SFML/Config.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/System/Vector2.hpp"
#include "../include/constants.h"
#include <catch2/catch.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <string>
#include <thread>


TEST_CASE("Words provider works as intented.") {

    SECTION("Default WordsProvider") {
        WordsProvider wp{};
        REQUIRE(wp.num_of_words() == 1000);
        REQUIRE_FALSE(wp.get_word().empty());
    }
    SECTION("Non-default WordsProvider with bad files") {
        const std::string bad_filename = "../bad/path";
        const std::string bad_filename_error =
            fmt::format("Could not open file where words are supposed to be: {}", bad_filename);
        REQUIRE_THROWS_WITH(WordsProvider{bad_filename}, bad_filename_error);

        const std::string good_filename = "../assets/google-0-english.txt";
        const std::string good_filename_error =
            fmt::format("No words loaded from the file, check if there are any: {}", good_filename);
        REQUIRE_THROWS_WITH(WordsProvider{good_filename}, good_filename_error);
    }
    SECTION("Non-default WordsProvider with good file") {
        WordsProvider wp{"../assets/google-10000-english.txt"};
        REQUIRE(wp.num_of_words() == 10000);
        REQUIRE_FALSE(wp.get_word().empty());
    }
}

auto load_font() {
    sf::Font font;
    if (!font.loadFromFile("../assets/ubuntu-font-family-0.83/"
                           "ubuntu-font-family-0.83/Ubuntu-L.ttf")) {
        fmt::print(fg(fmt::color::red), "font loading failed\n");
    }
    return font;
}

TEST_CASE("WordEntity") {
    using speedtyper::WordEntity;
    SECTION("Init and placement") {
        using namespace std::string_literals; // for ""s to create string
        auto text = "Hello"s;
        auto font = load_font();

        auto total_width = 0.0F;
        for (const auto& c : text) {
            total_width += font.getGlyph(static_cast<sf::Uint32>(c), speedtyper::GUI_options::ent_font_sz, false).advance;
        }

        WordEntity we{10.0F, 10.0F, text, font};

        REQUIRE(we.get_width_of_bbox() == total_width);

        auto new_pos = sf::Vector2f{20.0F, 20.0F};
        we.set_position(new_pos);
        REQUIRE(we.get_position() == new_pos);
    }
}

TEST_CASE("DisplayedWords") {
    using speedtyper::DisplayedWords;
    SECTION("Init") {
        auto font = load_font();
        DisplayedWords dw{font};
        dw.get_current_word().get_string();
    }
}

TEST_CASE("Timer") {
    using speedtyper::Timer;
    using namespace std::chrono_literals;
    constexpr auto initial_val = 42;
    SECTION("Timer will do correct timeout outcome."){
        auto num_for_test = initial_val;
        auto add_two_to_captured = [&num_for_test](){
            num_for_test += 2;
        };
        Timer t1;
        [[maybe_unused]] auto id_1 = t1.set_timeout(add_two_to_captured, 100ms);
        std::this_thread::sleep_for(200ms);
        REQUIRE(num_for_test == initial_val + 2);
    }

    SECTION("Timer timeout callback can be disabled."){
        auto num_for_test = initial_val;
        auto add_two_to_captured = [&num_for_test](){
            num_for_test += 2;
        };
        Timer t1;
        auto id_1 = t1.set_timeout(add_two_to_captured, 100ms);
        std::this_thread::sleep_for(50ms);
        t1.disable(id_1);
        std::this_thread::sleep_for(100ms);
        REQUIRE(num_for_test == initial_val);  
    }

    SECTION("Replace the task of the timer."){
        auto num_for_test = initial_val;
        auto add_two_to_captured = [&num_for_test](){
            num_for_test += 2;
        };
        auto add_four_to_captured = [&num_for_test](){
            num_for_test += 4;
        };
        Timer t1;
        auto id_1 = t1.set_timeout(add_two_to_captured, 100ms);
        std::this_thread::sleep_for(50ms);
        t1.disable(id_1);
        [[maybe_unused]]auto id_2 = t1.set_timeout(add_four_to_captured, 100ms);
        std::this_thread::sleep_for(200ms);

        REQUIRE(num_for_test == initial_val+4);  
    }

    SECTION("Replace the task of the timer several times."){
        auto num_for_test = initial_val;
        auto add_two_to_captured = [&num_for_test](){
            num_for_test += 2;
        };
        auto add_four_to_captured = [&num_for_test](){
            num_for_test += 4;
        };
        auto add_six_to_captured = [&num_for_test](){
            num_for_test += 6;
        };
        Timer t1;
        auto id_1 = t1.set_timeout(add_two_to_captured, 100ms);
        std::this_thread::sleep_for(50ms);
        t1.disable(id_1);
        auto id_2 = t1.set_timeout(add_four_to_captured, 100ms);
        std::this_thread::sleep_for(50ms);
        t1.disable(id_2);
        [[maybe_unused]]auto id_3 = t1.set_timeout(add_six_to_captured, 100ms);
        std::this_thread::sleep_for(200ms);

        REQUIRE(num_for_test == initial_val+6);  
    }
    SECTION("Timer can handle more than one task.") {
        auto num_for_test = initial_val;
        auto add_two_to_captured = [&num_for_test](){
            num_for_test += 2;
        };
        auto add_four_to_captured = [&num_for_test](){
            num_for_test += 4;
        };
        Timer t1;
        [[maybe_unused]] auto id_1 = t1.set_timeout(add_two_to_captured, 50ms);
        [[maybe_unused]] auto id_2 = t1.set_timeout(add_four_to_captured, 50ms);
        std::this_thread::sleep_for(100ms);
        REQUIRE(num_for_test == initial_val + 2 + 4);
    }
}

