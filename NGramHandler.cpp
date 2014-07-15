#include <iostream>
#include "NGramHandler.h"

const unsigned short NGramHandler::kMinOccurences = 2;
const size_t NGramHandler::kMaxSingleWordSymbols = 63000;
const size_t NGramHandler::kMaxMultiwordSymbols = 63000;

NGramHandler::NGramHandler(unsigned short max_words, Globals &globals_manager) :
    max_words_(max_words),
    globals_manager_(globals_manager),
    single_word_symbols_count_(0u),
    multi_word_symbols_count_(0u)
{
    occurence_counts_.resize(max_words_); //a dedicated map for each n-gram
}

void NGramHandler::ExtractAndStoreNGrams(const std::vector<Symbol>& sentence_tokens)
{
    const size_t sentence_size = sentence_tokens.size();

    /*
    // first count all single word symbols
    std::unordered_map<Symbol, size_t>& single_word_counts = occurence_counts_[0];
    for (size_t i = 0; i < sentence_size; ++i) {
        if (single_word_counts[sentence_tokens[i]]++ == 1u)
            ++single_word_symbols_count_;
    }
    */

    // then count multi-word symbols
    if (sentence_size >= max_words_) {
        for (unsigned short n_words = 2; n_words <= max_words_; ++n_words) {
            for (unsigned short i = 0; i <= sentence_size - n_words; ++i) {

                std::string compound_word(sentence_tokens[i]);
                for (unsigned short j = i + 1; j < i + n_words; ++j) {
                    //std::cout << "j = " << j << ", i = " << i << ", n_words = " << n_words << ", sentence_size = " << sentence_size << std::endl << std::flush;
                    compound_word += " ";
                    compound_word += sentence_tokens[j];
                }

                std::unordered_map<Symbol, size_t>& n_word_counts = occurence_counts_[n_words - 1];
                if (n_word_counts[compound_word]++ == 1u) {
                    ++multi_word_symbols_count_;
                    //std::cout << "Multi-word symbols are now: " << multi_word_symbols_estimate_ << std::endl;
                }
            }
        }
    }
}

void NGramHandler::CleanupNGrams()
{
    for (unsigned short n_words = 2; n_words <= max_words_; ++n_words) {
        std::unordered_map<Symbol, size_t>::iterator it = occurence_counts_[n_words - 1].begin();
        while (it != occurence_counts_[n_words - 1].end()) {
            std::unordered_map<Symbol, size_t>::iterator current = it++;
            if (current->second < kMinOccurences)
                occurence_counts_[n_words - 1].erase(current);
        }
    }
}
