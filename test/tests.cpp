#include "../src/DisplayedWords.h"
#include "../src/WordEntity.h"
#include "../src/WordsProvider.h"
#include "SFML/Config.hpp"
#include "SFML/Graphics/Font.hpp"
#include "SFML/System/Vector2.hpp"
#include <catch2/catch.hpp>
#include <fmt/color.h>
#include <fmt/core.h>
#include <string>

unsigned int Factorial(unsigned int number) {
    return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]") {
    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);
}

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
    SECTION("Init and placement") {
        using namespace std::string_literals; // for ""s to create string
        auto text = "Hello"s;
        auto font = load_font();
        auto font_sz = WordEntity::get_font_size();

        auto total_width = 0.0F;
        for (const auto& c : text) {
            total_width += font.getGlyph(static_cast<sf::Uint32>(c), font_sz, false).advance;
        }

        WordEntity we{10.0F, 10.0F, text, font};

        REQUIRE(we.get_width_of_bbox() == total_width);

        auto new_pos = sf::Vector2f{20.0F, 20.0F};
        we.set_position(new_pos);
        REQUIRE(we.get_position() == new_pos);
    }
}

TEST_CASE("DisplayedWords") {
    SECTION("Init") {
        auto font = load_font();
        DisplayedWords dw{font};
        dw.get_current_word().get_string();
    }
}
