/*
 * Copyright 2014 Periklis G. Liaskovitis
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

#ifndef NGRAMHANDLER_H
#define NGRAMHANDLER_H

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "SymbolMapping.h"

struct StringVector_Cmp
{
    bool operator() (const std::vector<std::string>& a, const std::vector<std::string> b)
    {
        std::string a_string("");
        for (const std::string& e: a)
            a_string += e;

        std::string b_string("");
        for (const std::string& e: b)
            b_string += e;

        return strcmp(a_string.c_str(), b_string.c_str()) < 0;
    }
};

struct Occurrence_Cmp
{
    bool operator() (const std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator& a,
                     const std::map<std::vector<std::string>, size_t, StringVector_Cmp>::iterator& b)
    {
        return (a->second < b->second);
    }
};

class NGramHandler
{
public:
    NGramHandler(uint8_t max_multi_words, uint8_t min_single_occurences, uint8_t min_multi_occurences);
    NGramHandler(const NGramHandler& rhs) = delete;
    NGramHandler& operator=(const NGramHandler& rhs) = delete;
    NGramHandler(NGramHandler&& rhs) = delete;
    NGramHandler&& operator=(NGramHandler&& rhs) = delete;

    // Counts occurences of n-grams from a given input sentence
    void ExtractAndStoreNGrams(const std::vector<std::string>& sentence_tokens);

    // Erases n-grams with occurence count less than the accepted threshold
    void CleanupNGrams();

    size_t get_single_word_count();
    size_t get_multi_word_count();

    std::unique_ptr<SymbolMapping> GetSingleWordSymbols();
    std::unique_ptr<SymbolMapping> GetMultiWordSymbols();
    std::unique_ptr<SymbolMapping> GetAllSymbols();

private:
    static const size_t kMaxSingleWordSymbols;
    static const size_t kMaxMultiWordSymbols;

    const uint8_t max_multi_words_;
    const uint8_t min_single_occurences_;
    const uint8_t min_multi_occurences_;

    std::vector<std::map<std::vector<std::string>, size_t, StringVector_Cmp>> occurrence_counts_;

};

#endif // NGRAMHANDLER_H
