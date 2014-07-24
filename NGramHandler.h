#ifndef NGRAMHANDLER_H
#define NGRAMHANDLER_H

#include <string>
#include <vector>
#include <unordered_map>

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

class Globals;

class NGramHandler
{
public:
    NGramHandler(unsigned short max_words, Globals& globals_manager);

    NGramHandler(const NGramHandler& rhs) = delete;
    NGramHandler& operator=(const NGramHandler& rhs) = delete;
    NGramHandler(NGramHandler&& rhs) = delete;
    NGramHandler&& operator=(NGramHandler&& rhs) = delete;

    // Counts occurences of n-grams from a given input sentence
    void ExtractAndStoreNGrams(const std::vector<Symbol>& sentence_tokens);

    // Erases n-grams with occurence count less than the accepted threshold
    void CleanupNGrams();

    size_t get_single_word_count() { return occurence_counts_[0].size(); }
    size_t get_multi_word_count() {
        size_t multi_word_symbols_count = 0;

        for (size_t i = 1; i < occurence_counts_.size(); ++i)
            multi_word_symbols_count += occurence_counts_[i].size();

        return multi_word_symbols_count;
    }

private:
    static const unsigned short kMinOccurences;
    static const size_t kMaxSingleWordSymbols;
    static const size_t kMaxMultiwordSymbols;

    const unsigned short max_words_;

    Globals& globals_manager_;
    std::vector<std::unordered_map<Symbol, size_t>> occurence_counts_;
};

#endif // NGRAMHANDLER_H
