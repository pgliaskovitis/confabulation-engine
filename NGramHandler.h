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

private:
    static const unsigned short kMinOccurences;
    static const size_t kMaxSingleWordSymbols;
    static const size_t kMaxMultiwordSymbols;

    const unsigned short max_words_;
    Globals& globals_manager_;
    std::vector<std::unordered_map<Symbol, size_t>> occurence_counts_;
};

#endif // NGRAMHANDLER_H
