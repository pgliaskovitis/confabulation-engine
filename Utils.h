#ifndef UTILS_H
#define UTILS_H

#include <cmath>

namespace
{
    bool IsNearlyEqual(float x, float y)
    {
        const double epsilon = 1e-5;
            return std::abs(x - y) <= epsilon * std::abs(x);
            // see Knuth section 4.2.2 pages 217-218
    }
}

#endif // UTILS_H
