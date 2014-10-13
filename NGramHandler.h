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

#ifndef DEBUG_1_H_
//#define DEBUG_1_H_
#endif

#ifndef CONFSYMBOL_H_
#define CONFSYMBOL_H_
//the basic item of knowledge in this framework
typedef std::string Symbol;
#endif

struct StringVector_Cmp
{
    bool operator()(const std::vector<Symbol>& a, const std::vector<Symbol> b)
    {
        std::string a_string("");
        for (const Symbol& e: a)
            a_string += e;

        std::string b_string("");
        for (const Symbol& e: b)
            b_string += e;

        return strcmp(a_string.c_str(), b_string.c_str()) < 0;
    }
};

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

    size_t get_single_word_count();
    size_t get_multi_word_count();

    std::unique_ptr<SymbolMapping> GetSingleWordSymbols();
    std::unique_ptr<SymbolMapping> GetAllSymbols();

private:
    static const unsigned short kMinOccurences;
    static const size_t kMaxSingleWordSymbols;
    static const size_t kMaxMultiwordSymbols;

    const unsigned short max_words_;

    Globals& globals_manager_;
    std::vector<std::map<std::vector<Symbol>, size_t, StringVector_Cmp>> occurrence_counts_;

};

#endif // NGRAMHANDLER_H
