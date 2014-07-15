#include "NGramHandler.h"

const unsigned short NGramHandler::kMinOccurences = 2;
const size_t NGramHandler::kMaxSingleWordSymbols = 63000;
const size_t NGramHandler::kMaxMultiwordSymbols = 63000;

NGramHandler::NGramHandler(unsigned short max_words, Globals &globals_manager) : max_words_(max_words), globals_manager_(globals_manager)
{
    occurence_counts_.resize(max_words_); //a dedicated map for each n-gram
}

void NGramHandler::ExtractAndStoreNGrams(const std::vector<Symbol>& sentence_tokens)
{
    const size_t sentence_size = sentence_tokens.size();

    // first count all single word symbols
    std::unordered_map<Symbol, size_t>& single_word_counts = occurence_counts_[0];
    for (size_t i = 0; i < sentence_size; ++i)
        single_word_counts[sentence_tokens[i]]++;

    // then count multi-word symbols
    for (unsigned short n_words = 2; n_words < max_words_; ++n_words) {
        for (unsigned short i = 0; i <= sentence_size - n_words; ++i) {

            std::string compound_word(sentence_tokens[i]);
            for (unsigned short j = i + 1; j < i + n_words; ++j) {
                compound_word += " ";
                compound_word += sentence_tokens[j];
            }

            std::unordered_map<Symbol, size_t>& n_word_counts = occurence_counts_[n_words - 1];
            n_word_counts[compound_word]++;
        }
    }

}


