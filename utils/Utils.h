#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <sstream>

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

    // each row of the 2D argument vector has as many elements as the corresponding level of the architecture
    // each element corresponds to exactly one module in the architecture
    std::vector<std::vector<std::string>> ProduceKnowledgeLinkCombinations(const std::vector<std::vector<std::string>>& excited_symbols)
    {
        std::vector<std::vector<std::string>> results;

        size_t total_size = 0;

        for (size_t i = 0; i < excited_symbols.size(); ++i)
            total_size += excited_symbols[i].size();

        // total size is now equal to the total number of modules in the architecture
        results.resize(excited_symbols[0].size() - 1);
        for (size_t k = 0; k < excited_symbols[0].size() - 1; ++k) {
            // each result vector addresses all modules
            results[k].resize(total_size, "");

            // special care must be taken because the different levels of the architecture need not be of the same size
            size_t level_size = 0;
            for (size_t i = 0; i < excited_symbols.size(); ++i) {
                for (size_t j = 0; j < excited_symbols[i].size(); ++j) {
                    if (j + k < excited_symbols[i].size())
                        results[k][level_size + j + k] = excited_symbols[i][j];
                }
                level_size += excited_symbols[i].size();
            }
        }

        return results;
    }
}

#endif // UTILS_H
