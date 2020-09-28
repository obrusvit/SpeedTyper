#ifndef GUARD_SPEED_TYPER_WORDS_PROVIDER
#define GUARD_SPEED_TYPER_WORDS_PROVIDER

#include <string>
#include <vector>

class WordsProvider {

public:

    WordsProvider();
    explicit WordsProvider(std::string filename);

    [[nodiscard]] const std::string& get_word() const;
    [[nodiscard]] int num_of_words() const;

    void print_words() const;

private:
    [[nodiscard]] int get_rand_int_uniform() const;

    const std::string _filename;
    std::vector<std::string> _words;
};

#endif

