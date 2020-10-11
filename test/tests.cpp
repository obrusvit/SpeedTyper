#include "fmt/core.h"
#include <catch2/catch.hpp>

unsigned int Factorial(unsigned int number)
{
    return number <= 1 ? number : Factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed", "[factorial]")
{
    REQUIRE(Factorial(1) == 1);
    REQUIRE(Factorial(2) == 2);
    REQUIRE(Factorial(3) == 6);
    REQUIRE(Factorial(10) == 3628800);
}

#include <fmt/core.h>
#include "../src/WordsProvider.h"

TEST_CASE("Words provider works as intented."){

    SECTION("Default WordsProvider"){
        WordsProvider wp{};
        REQUIRE( wp.num_of_words() == 1000 );
        REQUIRE_FALSE( wp.get_word().empty());
    }
    SECTION("Non-default WordsProvider with bad files"){
        const std::string bad_filename = "../bad/path";
        const std::string bad_filename_error = fmt::format("Could not open file where words are supposed to be: {}", bad_filename);
        REQUIRE_THROWS_WITH(WordsProvider{bad_filename}, bad_filename_error);

        const std::string good_filename = "../assets/google-0-english.txt";
        const std::string good_filename_error = fmt::format("No words loaded from the file, check if there are any: {}", good_filename);
        REQUIRE_THROWS_WITH(WordsProvider{good_filename}, good_filename_error);
    }
    SECTION("Non-default WordsProvider with good file"){
        WordsProvider wp {"../assets/google-10000-english.txt"};
        REQUIRE( wp.num_of_words() == 10000 );
        REQUIRE_FALSE( wp.get_word().empty());
    }
}
