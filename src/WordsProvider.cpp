#include "WordsProvider.h"
#include <fstream>
#include <algorithm>
#include <random>
#include <fmt/core.h>
#include <fmt/color.h>
#include <stdexcept>
#include <string>
#include <vector>

WordsProvider::WordsProvider() 
    : WordsProvider{"../assets/google-1000-english.txt"}
{
}

WordsProvider::WordsProvider(std::string filename)
    : _filename{std::move(filename)}
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
    auto idx = get_rand_idx_uniform();
    return _words.at(idx);
}

void WordsProvider::print_words() const {
    for (int cnt = 1; const auto& w : _words){
        fmt::print("{}: {}\n", cnt, w);
        ++cnt;
    }
}

// private
long unsigned int WordsProvider::get_rand_idx_uniform() const {
    // Seed with a real random value, if available
    static std::random_device r;
 
    static std::default_random_engine e1(r());
    static const auto upper_limit = std::max<long unsigned int>(0, num_of_words()-1);  //prevent invalid dist range
    static std::uniform_int_distribution<long unsigned int> uniform_dist(0, upper_limit);
 
    // Generate a normal distribution around that mean
    static std::seed_seq seed2{r(), r(), r(), r(), r(), r(), r(), r()}; 
    static std::mt19937 e2(seed2);
    return uniform_dist(e2);
}



