#include "words_provider.hpp"
#include <fstream>
#include <algorithm>
#include <random>
#include <fmt/core.h>
#include <fmt/color.h>
#include <stdexcept>

WordsProvider::WordsProvider() 
    : WordsProvider{"../assets/google-1000-english.txt"}
{
}

WordsProvider::WordsProvider(std::string filename)
    : _filename{std::move(filename)}
    , _words{}
{
    std::ifstream ifs{_filename};
    if(!ifs){
        fmt::print(fg(fmt::color::red), "Filename called '{}' was not found.\n", _filename);
        throw std::runtime_error(fmt::format("Could not open file where words are supposed to be: {}", _filename));
    }
    std::string s;
    while(ifs >> s){
        _words.push_back(s);
    }
    if (_words.empty()){
        throw std::runtime_error(fmt::format("No words loaded from the file, check if there are any: {}", _filename));
    }
}


const std::string& WordsProvider::get_word() const {
    int idx = get_rand_int_uniform();
    return _words.at(idx);
}

int WordsProvider::num_of_words() const {
    return _words.size();
}

void WordsProvider::print_words() const {
    for (int cnt = 1; const auto& w : _words){
        fmt::print("{}: {}\n", cnt, w);
        ++cnt;
    }
}

// private
int WordsProvider::get_rand_int_uniform() const {
    // Seed with a real random value, if available
    static std::random_device r;
 
    static std::default_random_engine e1(r());
    static const int upper_limit = std::max<int>(0, _words.size()-1);  //prevent segfault by invalid dist range
    static std::uniform_int_distribution<int> uniform_dist(0, upper_limit);
 
    // Generate a normal distribution around that mean
    static std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()}; 
    static std::mt19937 e2(seed2);
    return uniform_dist(e2);
}



