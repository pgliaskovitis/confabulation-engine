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

#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <sstream>
#include <limits>
#include <iostream>

namespace
{
    bool IsNearlyEqual(float x, float y)
    {
        const double epsilon = 1e-6;
            return std::abs(x - y) <= epsilon * std::abs(x);
            // see Knuth section 4.2.2 pages 217-218
    }

    size_t BinarySearch(const std::vector<size_t>::const_iterator& begin_it, const std::vector<size_t>::const_iterator& end_it,
                        size_t key, bool& found)
    {
        std::vector<size_t>::const_iterator lower = begin_it;
        std::vector<size_t>::const_iterator upper = end_it - 1;
        while (lower < upper) {
            std::vector<size_t>::const_iterator mid = lower + (upper - lower) / 2;

            if (key == *mid) {
                found = true;
                return (mid - begin_it);
            }

            if (key < *mid)
                upper = mid - 1;
            else
                lower = mid + 1;
        }

        size_t result = lower - begin_it;

        if (key == *(lower + result))
            found = true;

        return result;
    }

    template <typename T>
    T PositiveClip(T x) {
        return x > 0 ? x : 0;
    }

    std::string VectorSymbolToSymbol(const std::vector<std::string>& vector_symbol, char delim)
    {
        std::string result("");

        size_t symbol_count = vector_symbol.size();
        size_t i = 0;
        for (const std::string& e: vector_symbol) {
            result += e;
            if (i < symbol_count - 1)
                result += delim;
            ++i;
        }

        return result;
    }

    std::string ListSymbolToSymbol(const std::list<std::string>& list_symbol, char delim)
    {
        std::string result("");

        size_t symbol_count = list_symbol.size();
        size_t i = 0;
        for (const std::string& e: list_symbol) {
            result += e;
            if (i < symbol_count - 1)
                result += delim;
            ++i;
        }

        return result;
    }

    std::string SetSymbolToSymbol(const std::set<std::string>& set_symbol, char delim)
    {
        std::string result("");

        size_t symbol_count = set_symbol.size();
        size_t i = 0;
        for (const std::string& e: set_symbol) {
            result += e;
            if (i < symbol_count - 1)
                result += delim;
            ++i;
        }

        return result;
    }

    std::vector<std::string> SymbolToVectorSymbol(const std::string& symbol, char delim)
    {
        std::vector<std::string> result;

        std::stringstream ss(symbol);
        std::string item;
        while (std::getline(ss, item, delim)) {
            result.push_back(item);
        }

        return result;
    }

    int ConvertToSigned(unsigned int x)
    {
        if (x <= std::numeric_limits<int>::max())
            return static_cast<int>(x);

        if (x >= std::numeric_limits<int>::min())
            return static_cast<int>(x - std::numeric_limits<int>::min()) + std::numeric_limits<int>::min();

        throw x;
    }

    std::vector<std::string> FillWithEmptyStrings(const std::vector<std::string>& input, size_t max_size)
    {
        std::vector<std::string> result(input);
        if (input.size() < max_size) {
            for (size_t i = result.size(); i < max_size; ++i)
                result.push_back("");
        }

        return result;
    }

    size_t FindNumberOfEmptyStringsBeforeIndex(const std::vector<std::string>& symbols, int index)
    {
        size_t result = 0;
        for (int i = 0; i < index; ++i)
            if (symbols[i].empty())
                ++result;

        return result;
    }

    int FindFirstIndexOfSymbol(const std::vector<std::string>& symbols, const std::string& target_string)
    {
        for (size_t i = 0; i < symbols.size(); ++i)
            if (symbols[i] == target_string)
                return ConvertToSigned(i);

        return -1;
    }

    int FindFirstIndexNotOfSymbol(const std::vector<std::string>& symbols, const std::string& target_string)
    {
        for (size_t i = 0; i < symbols.size(); ++i)
            if (symbols[i] != target_string)
                return ConvertToSigned(i);

        return -1;
    }

    // The purpose of this function is to produce all possible combinations of symbol activations for the given
    // initial symbol activation within a given multi-level architecture
    std::vector<std::vector<std::string>> ProduceKnowledgeLinkCombinations(const std::vector<std::vector<std::string>>& excited_symbols, unsigned short num_modules)
    {
        std::vector<std::vector<std::string>> results;

        int total_size = 0;

        for (size_t i = 0; i < excited_symbols.size(); ++i) {
            // each row of the argument vector should have as many elements as
            // exist in the corresponding level of the architecture
            total_size += ConvertToSigned(excited_symbols[i].size());
        }

        // total size should be equal to the total number of modules in the architecture
        assert(total_size == num_modules);

        int convolution_half_size = ConvertToSigned(excited_symbols[0].size() - 1);
        results.resize(2 * convolution_half_size - 2);
        for (int k = -convolution_half_size + 2; k < convolution_half_size; ++k) {
            // each result vector addresses all modules
            results[k + convolution_half_size - 2].resize(total_size, "");

            // the given sentence tokens are "convolved" with the modules of the architecture
            // as they are shifted from the far left to the far right
            // special care must be taken, since the different levels of the architecture need not be of the same size
            int level_size = 0;
            for (size_t i = 0; i < excited_symbols.size(); ++i) {
                for (size_t j = 0; j < excited_symbols[i].size(); ++j) {
                    if ((ConvertToSigned(j) + k >= 0) &&
                        (ConvertToSigned(j) + k < ConvertToSigned(excited_symbols[i].size()))) {
                        results[k + convolution_half_size - 2][level_size + ConvertToSigned(j) + k] = excited_symbols[i][j];
                    }
                }
                level_size += ConvertToSigned(excited_symbols[i].size());
            }
        }

        return results;
    }

}

#endif // UTILS_H
