#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <string>
#include <vector>

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
}

#endif // UTILS_H
