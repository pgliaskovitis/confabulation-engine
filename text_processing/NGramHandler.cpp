/*
 * Copyright 2014 Periklis Liaskovitis
 *
 * This file is part of confab-engine.
 *
 * confab-engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * confab-engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with confab-engine.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <stdexcept>
#include <queue>
#include <deque>
#include <algorithm>
#include "NGramHandler.h"
#include "utils/Utils.h"
#include "Dbg.h"

const size_t NGramHandler::kMaxSingleWordSymbols = 65000;
const size_t NGramHandler::kMaxMultiWordSymbols = 65000;

NGramHandler::NGramHandler(uint8_t max_multi_words, uint8_t min_single_occurences, uint8_t min_multi_occurences) :
    max_multi_words_(max_multi_words),
    min_single_occurences_(min_single_occurences),
    min_multi_occurences_(min_multi_occurences)
{
    occurrence_counts_.resize(max_multi_words_); //a dedicated map for each n-gram
}

void NGramHandler::ExtractAndStoreNGrams(const std::vector<std::string>& sentence_tokens)
{
    const size_t sentence_size = sentence_tokens.size();

    // count occurrences of single words, to serve as basis
    std::map<std::vector<std::string>, size_t, StringVector_Cmp>& single_word_counts = occurrence_counts_[0];
    for (size_t i = 0; i < sentence_size; ++i) {
        std::vector<std::string> single_word_vec;
        if (!sentence_tokens[i].empty()) {
            single_word_vec.push_back(sentence_tokens[i]);
            single_word_counts[single_word_vec]++;
        }
    }

    // for each possible multiword length,
    // count occurrences whose prefixes occured at least MIN_OCC times
    if (sentence_size >= max_multi_words_) {
        for (uint8_t n_words = 2; n_words <= max_multi_words_; ++n_words) {
            for (size_t i = 0; i <= sentence_size - n_words; ++i) {

                std::vector<std::string> compound_word;
                if (!sentence_tokens[i].empty()) {
                    compound_word.push_back(sentence_tokens[i]);
                    for (size_t j = i + 1; j < i + n_words; ++j) {
                        compound_word.push_back(sentence_tokens[j]);
                    }

                    std::map<std::vector<std::string>, size_t, StringVector_Cmp>& n_word_counts = occurrence_counts_[n_words - 1];
                    n_word_counts[compound_word]++;
                }
            }
        }
    }
}

void NGramHandler::CleanupNGrams()
{
    // for each possible multiword length,
    // remove occurrences whose prefixes occured fewer than MIN_OCC times
    for (uint8_t n_words = 2; n_words <= max_multi_words_; ++n_words) {
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>& current_occ_count = occurrence_counts_[n_words - 1];
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>& prev_occ_count = occurrence_counts_[n_words - 2];
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it_end = current_occ_count.end();

        for (std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it = current_occ_count.begin(); it != it_end;) {

            std::vector<std::string>::const_iterator symbol_it = (it->first).begin();
            std::vector<std::string> prefix(symbol_it, symbol_it + n_words - 1);

/*
            std::string debug_output_1("");
            for (const std::string& e: it->first) {
                debug_output_1 += e;
                debug_output_1 += " ";
            }

            std::string debug_output_2("");
            for (const std::string& e: prefix) {
                debug_output_2 += e;
                debug_output_2 += " ";
            }
*/
            size_t prefix_count = 0;
            bool must_delete = false;
            try {
                prefix_count = prev_occ_count.at(prefix);
            } catch (std::out_of_range& oor) {
                must_delete = true;
            }

//            std::cout << "Prefix of multiword \"" << debug_output_1 << "\" is : \"" << debug_output_2 << "\" with occurrence count " << prefix_count << "\n";

            if (prefix_count < min_multi_occurences_) {
                must_delete = true;
            }

            if (must_delete) {
                current_occ_count.erase(it++);
            } else {
                ++it;
            }
        }
    }

    log_info("Finished stage I of cleaning up multigrams");

    // now only multiwords who have "sufficiently" occurring prefixes still exist in the maps
    // remove counts of longer groups from the count of the sub groups
    // never erase counts of single words
    for (uint8_t n_words = 3; n_words <= max_multi_words_; ++n_words) {
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>& current_occ_count = occurrence_counts_[n_words - 1];
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it_end = current_occ_count.end();
        for (uint8_t subgroup_words = n_words - 1; subgroup_words >= 2; --subgroup_words) {
            std::map<std::vector<std::string>, size_t, StringVector_Cmp>& prev_occ_count = occurrence_counts_[subgroup_words - 1];

            for (std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it = current_occ_count.begin(); it != it_end; ++it) {
                if (it->second >= min_multi_occurences_) {
                    std::vector<std::string>::const_iterator symbol_it = (it->first).begin();
                    std::vector<std::string> prefix(symbol_it, symbol_it + subgroup_words);

                    symbol_it = (it->first).end();
                    std::vector<std::string> suffix(symbol_it - subgroup_words, symbol_it);

                    prev_occ_count[prefix] -= it->second;
                    prev_occ_count[suffix] -= it->second;
                }
            }
        }
    }

    log_info("Finished stage II of cleaning up multigrams");

    // clean the words with less than min_single_occurrences
    for (uint8_t n_words = 1; n_words <=1; ++n_words) {
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it = occurrence_counts_[n_words - 1].begin();
        while (it != occurrence_counts_[n_words - 1].end()) {
            std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator current = it++;
            if (current->second < min_single_occurences_) {
                occurrence_counts_[n_words - 1].erase(current);
            }
        }
    }

    // clean the groups with less than min_multi_occurrences
    for (uint8_t n_words = 2; n_words <= max_multi_words_; ++n_words) {
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it = occurrence_counts_[n_words - 1].begin();
        while (it != occurrence_counts_[n_words - 1].end()) {
            std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator current = it++;
            if (current->second < min_multi_occurences_) {
                occurrence_counts_[n_words - 1].erase(current);
            }
        }
    }

    log_info("Finished stage III of cleaning up multigrams");

    // limit single and multiword counts to their chosen maximum values
    typedef std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator NGramWithCountIterator;
    std::deque<NGramWithCountIterator> queue;
    NGramWithCountIterator it = occurrence_counts_[0].begin();
    while (it != occurrence_counts_[0].end()) {
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator current = it++;
        queue.push_back(current);
    }

    Occurrence_Cmp occurence_cmp;
    std::sort(queue.begin(), queue.end(), occurence_cmp);

    while (queue.size() > kMaxSingleWordSymbols) {
        NGramWithCountIterator removed_ngram_it = queue.front();

        //std::cout << "Erasing NGram \"" << VectorSymbolToSymbol(removed_ngram_it->first, ' ') << "\" with count " << removed_ngram_it->second << "\n" << std::flush;

        NGramWithCountIterator current_it = occurrence_counts_[0].find(removed_ngram_it->first);
        occurrence_counts_[0].erase(current_it);
        queue.pop_front();
    }

    queue.clear();
    for (uint8_t n_words = 2; n_words <= max_multi_words_; ++n_words) {
        typedef std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator NGramWithCountIterator;
        NGramWithCountIterator it = occurrence_counts_[n_words - 1].begin();
        while (it != occurrence_counts_[n_words - 1].end()) {
            std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator current = it++;
            queue.push_back(current);
        }
    }

    std::sort(queue.begin(), queue.end(), occurence_cmp);

    while (queue.size() > kMaxMultiWordSymbols) {
        NGramWithCountIterator removed_ngram_it = queue.front();

        //std::cout << "Erasing NGram \"" << VectorSymbolToSymbol(removed_ngram_it->first, ' ') << "\" with count " << removed_ngram_it->second << "\n" << std::flush;

        for (uint8_t n_words = 2; n_words <= max_multi_words_; ++n_words) {
            NGramWithCountIterator current_it = occurrence_counts_[n_words - 1].find(removed_ngram_it->first);
            if (current_it != occurrence_counts_[n_words - 1].end()) {
                occurrence_counts_[n_words - 1].erase(current_it);
                break;
            }
        }
        queue.pop_front();
    }

    log_info("Finished stage IV of cleaning up multigrams");
}

size_t NGramHandler::get_single_word_count()
{
    return occurrence_counts_[0].size();
}

size_t NGramHandler::get_multi_word_count()
{
    size_t multi_word_symbols_count = 0;

    for (size_t i = 1; i < occurrence_counts_.size(); ++i) {
        multi_word_symbols_count += occurrence_counts_[i].size();
    }

    return multi_word_symbols_count;
}

std::unique_ptr<SymbolMapping> NGramHandler::GetSingleWordSymbols()
{
    std::unique_ptr<SymbolMapping> result(new SymbolMapping());

    std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it = occurrence_counts_[0].begin();
    for (; it != occurrence_counts_[0].end(); ++it) {
        result->AddSymbol(VectorSymbolToSymbol(it->first, ' '));
    }

    return result;
}

std::unique_ptr<SymbolMapping> NGramHandler::GetMultiWordSymbols()
{
    std::unique_ptr<SymbolMapping> result(new SymbolMapping());

    for (size_t i = 1; i < occurrence_counts_.size(); ++i) {
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it = occurrence_counts_[i].begin();
        for (; it != occurrence_counts_[i].end(); ++it) {
            result->AddSymbol(VectorSymbolToSymbol(it->first, ' '));
        }
    }

    return result;
}

std::unique_ptr<SymbolMapping> NGramHandler::GetAllSymbols()
{
    std::unique_ptr<SymbolMapping> result(new SymbolMapping());

    for (size_t i = 0; i < occurrence_counts_.size(); ++i) {
        std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator it = occurrence_counts_[i].begin();
        for (; it != occurrence_counts_[i].end(); ++it) {
            result->AddSymbol(VectorSymbolToSymbol(it->first, ' '));
        }
    }

    return result;
}




