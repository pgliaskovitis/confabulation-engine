#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <string>
#include <vector>
#include <list>
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

        for (const std::string& e: vector_symbol) {
            result += e;
            result += delim;
        }

        return result;
    }

    std::string ListSymbolToSymbol(const std::list<std::string>& list_symbol, char delim)
    {

        std::string result("");

        for (const std::string& e: list_symbol) {
            result += e;
            result += delim;
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

    int ConvertToSigned(unsigned x)
    {
        if (x <= std::numeric_limits<int>::max())
            return static_cast<int>(x);

        if (x >= std::numeric_limits<int>::min())
            return static_cast<int>(x - std::numeric_limits<int>::min()) + std::numeric_limits<int>::min();

        throw x;
    }

    // each row of the 2D argument vector has as many elements as
    // exist in the corresponding level of the architecture
    std::vector<std::vector<std::string>> ProduceKnowledgeLinkCombinations(const std::vector<std::vector<std::string>>& excited_symbols)
    {
        std::vector<std::vector<std::string>> results;

        int total_size = 0;

        // total size is equal to the total number of modules in the architecture
        for (size_t i = 0; i < excited_symbols.size(); ++i)
            total_size += ConvertToSigned(excited_symbols[i].size());

        int convolution_half_size = ConvertToSigned(excited_symbols[0].size() - 1);
        results.resize(2 * convolution_half_size - 2);
        for (int k = -convolution_half_size + 2; k < convolution_half_size; ++k) {
            // each result vector addresses all modules
            results[k + convolution_half_size - 2].resize(total_size, "");

            // the given sentence tokens are "convolved" with the modules of the architecture
            // as they are shifted from the far left to the far right
            // special care must be taken because the different levels of the architecture need not be of the same size
            int level_size = 0;
            for (size_t i = 0; i < excited_symbols.size(); ++i) {
                for (size_t j = 0; j < excited_symbols[i].size(); ++j) {
                    if ((ConvertToSigned(j) + k > 0) &&
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
