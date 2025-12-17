#pragma once
#include "core.h"

// TODO: Remove #pragma warning(disable : 4577) when we remove dependency to cmath
#pragma warning(disable : 4577) // 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed. Specify /EHsc
#include <cmath>

inline i32 RoundFloatToInt(f32 number)
{
    i32 result = (i32)(number + 0.5f);
    return result;
}

inline u32 RoundFloatToUInt(f32 number)
{
    u32 result = (u32)(number + 0.5f);
    return result;
}

inline i32 TruncateFloatToInt(f32 number)
{
    i32 result = (i32)number;
    return result;
}

inline i32 FloorFloatToInt(f32 number)
{
    i32 result = (i32)std::floor(number);
    return result;
}

inline i32 CeilFloatToInt(f32 number)
{
    i32 result = (i32)std::ceil(number);
    return result;
}

inline f32 MinFloat(f32 a, f32 b)
{
    return (a < b) ? a : b;
}

inline f32 MaxFloat(f32 a, f32 b)
{
    return (a > b) ? a : b;
}

inline i32 MinInt(i32 a, i32 b)
{
    return (a < b) ? a : b;
}

inline i32 MaxInt(i32 a, i32 b)
{
    return (a > b) ? a : b;
}