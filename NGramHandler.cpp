#include <iostream>
#include <stdexcept>
#include "NGramHandler.h"

const unsigned short NGramHandler::kMinOccurences = 2;
const size_t NGramHandler::kMaxSingleWordSymbols = 120000;
const size_t NGramHandler::kMaxMultiwordSymbols = 120000;

NGramHandler::NGramHandler(unsigned short max_words, Globals &globals_manager) :
    max_words_(max_words),
    globals_manager_(globals_manager)
{
    occurrence_counts_.resize(max_words_); //a dedicated map for each n-gram
}

void NGramHandler::ExtractAndStoreNGrams(const std::vector<Symbol>& sentence_tokens)
{
    const size_t sentence_size = sentence_tokens.size();

    // count occurrences of single words, to serve as basis
    std::map<std::vector<Symbol>, size_t>& single_word_counts = occurrence_counts_[0];
    for (size_t i = 0; i < sentence_size; ++i) {
        std::vector<Symbol> single_word_vec;
        single_word_vec.push_back(sentence_tokens[i]);
        single_word_counts[single_word_vec]++;
    }

    // for each possible multiword length,
    // count occurrences whose prefixes occured at least MIN_OCC times
    if (sentence_size >= max_words_) {
        for (unsigned short n_words = 2; n_words <= max_words_; ++n_words) {
            for (unsigned short i = 0; i <= sentence_size - n_words; ++i) {

                std::vector<std::string> compound_word;
                compound_word.push_back(sentence_tokens[i]);
                for (unsigned short j = i + 1; j < i + n_words; ++j)
                    compound_word.push_back(sentence_tokens[j]);

                std::map<std::vector<Symbol>, size_t>& n_word_counts = occurrence_counts_[n_words - 1];
                n_word_counts[compound_word]++;
            }
        }
    }
}

void NGramHandler::CleanupNGrams()
{
    // for each possible multiword length,
    // remove occurrences whose prefixes occured fewer than MIN_OCC times
    for (unsigned short n_words = 2; n_words <= max_words_; ++n_words) {
        std::map<std::vector<Symbol>, size_t>& current_occ_count = occurrence_counts_[n_words - 1];
        std::map<std::vector<Symbol>, size_t>& prev_occ_count = occurrence_counts_[n_words - 2];
        std::map<std::vector<Symbol>, size_t>::iterator it_end = current_occ_count.end();

        for (std::map<std::vector<Symbol>, size_t>::iterator it = current_occ_count.begin(); it != it_end;) {

            std::vector<Symbol>::const_iterator symbol_it = (it->first).begin();
            std::vector<Symbol> prefix(symbol_it, symbol_it + n_words - 2);

            size_t prefix_count = 0;
            bool must_delete = false;
            try {
                prefix_count = prev_occ_count.at(prefix);
            } catch (std::out_of_range& oor) {
                must_delete = true;
            }

            if (prefix_count < kMinOccurences)
                must_delete = true;

            if (must_delete)
                current_occ_count.erase(it++);
            else
                ++it;
        }
    }

    // now only multiwords who have "sufficiently" occurring prefixes still exist in the maps
    // remove counts of longer groups from the count of the sub groups
    // never erase counts of single words
    for (unsigned short n_words = 3; n_words <= max_words_; ++n_words) {
        std::map<std::vector<Symbol>, size_t>& current_occ_count = occurrence_counts_[n_words - 1];
        std::map<std::vector<Symbol>, size_t>::iterator it_end = current_occ_count.end();
        for (unsigned short subgroup_words = n_words - 1; subgroup_words >= 2; --subgroup_words) {
            std::map<std::vector<Symbol>, size_t>& prev_occ_count = occurrence_counts_[subgroup_words - 1];

            for (std::map<std::vector<Symbol>, size_t>::iterator it = current_occ_count.begin(); it != it_end; ++it) {
                if (it->second >= kMinOccurences) {
                    std::vector<Symbol>::const_iterator symbol_it = (it->first).begin();
                    std::vector<Symbol> prefix(symbol_it, symbol_it + subgroup_words);

                    symbol_it = (it->first).end();
                    std::vector<Symbol> suffix(symbol_it, symbol_it - subgroup_words);

                    prev_occ_count[prefix] -= it->second;
                    prev_occ_count[suffix] -= it->second;
                }
            }
        }
    }

    // clean the groups with less than MIN_OCC occurrences
    // start at two: keep all the single word symbols
    for (unsigned short n_words = 2; n_words <= max_words_; ++n_words) {
        std::map<std::vector<Symbol>, size_t>::iterator it = occurrence_counts_[n_words - 1].begin();
        while (it != occurrence_counts_[n_words - 1].end()) {
            std::map<std::vector<Symbol>, size_t>::iterator current = it++;
            if (current->second < kMinOccurences) {
                occurrence_counts_[n_words - 1].erase(current);
            }
        }
    }

    // TODO -- limit single and multiword counts to their chosen maximum values
}




