#include "utils.h"

namespace {

bool IsNearlyEqual(int x, int y)
{
    return (x == y);
}

bool IsNearlyEqual(float x, float y)
{
    const double epsilon = 1e-5;
        return std::abs(x - y) <= epsilon * std::abs(x);
        // see Knuth section 4.2.2 pages 217-218
}

}
