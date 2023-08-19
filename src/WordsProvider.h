#ifndef SPEED_TYPER_WORDS_PROVIDER
#define SPEED_TYPER_WORDS_PROVIDER

#include <string>
#include <vector>

class WordsProvider {

public:

    WordsProvider();
    explicit WordsProvider(std::string filename);

    [[nodiscard]] const std::string& get_word() const;
    [[nodiscard]] auto num_of_words() const {
        return _words.size();
    }

    void print_words() const;

private:
    [[nodiscard]] long unsigned int get_rand_idx_uniform() const;

    std::string _filename;
    std::vector<std::string> _words;
};

#endif  /* ifndef SPEED_TYPER_WORDS_PROVIDER */
