#pragma once
#include "core.h"

// Define our Handle type
typedef u32 ResourceID;

// Invalid ID constant for error checking
const ResourceID INVALID_RESOURCE_ID = 0;

// FNV-1a Hash Algorithm (Converts String -> uint32_t)
// "constexpr" allows the compiler to pre-calculate hashes for string literals!
constexpr ResourceID HashString(const char* str)
{
    u32 hash = 2166136261u;
    for (int i = 0; str[i] != 0; ++i)
    {
        hash ^= (unsigned char)str[i];
        hash *= 16777619;
    }
    return hash;
}