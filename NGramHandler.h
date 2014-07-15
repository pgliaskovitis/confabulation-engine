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

    //Counts occurences of n-grams from a given input sentence
    void ExtractAndStoreNGrams(const std::vector<Symbol>& sentence_tokens);

    // Erases n-grams withoccurence count less than the accepted threshold
    void CleanupNGrams();

    size_t get_single_word_count() { return single_word_symbols_count_; }
    size_t get_multi_word_count() { return multi_word_symbols_count_; }

private:
    static const unsigned short kMinOccurences;
    static const size_t kMaxSingleWordSymbols;
    static const size_t kMaxMultiwordSymbols;

    const unsigned short max_words_;

    Globals& globals_manager_;
    std::vector<std::unordered_map<Symbol, size_t>> occurence_counts_;

    size_t single_word_symbols_count_;
    size_t multi_word_symbols_count_;
};

#endif // NGRAMHANDLER_H
