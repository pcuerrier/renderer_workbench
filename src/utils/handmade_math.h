#pragma once
#include "core.h"

// TODO: Remove #pragma warning(disable : 4577) when we remove dependency to cmath
#pragma warning(disable : 4577) // 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
#include <cmath>

inline int RoundFloatToInt(float number)
{
    int result = (int)(number + 0.5f);
    return result;
}

inline unsigned int RoundFloatToUInt(float number)
{
    unsigned int result = (unsigned int)(number + 0.5f);
    return result;
}

inline int TruncateFloatToInt(float number)
{
    int result = (int)number;
    return result;
}

inline int FloorFloatToInt(float number)
{
    int result = (int)std::floor(number);
    return result;
}

inline int CeilFloatToInt(float number)
{
    int result = (int)std::ceil(number);
    return result;
}

inline float MinFloat(float a, float b)
{
    return (a < b) ? a : b;
}

inline float MaxFloat(float a, float b)
{
    return (a > b) ? a : b;
}

inline int MinInt(int a, int b)
{
    return (a < b) ? a : b;
}

inline int MaxInt(int a, int b)
{
    return (a > b) ? a : b;
}