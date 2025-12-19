#pragma once

#include "core.h"

struct MemoryArena
{
    u64 size;
    u64 used;
    void* base;

    // TODO: Add linking to other arenas for more flexibility
};

struct Memory
{
    MemoryArena permanent_storage;
    MemoryArena transient_storage;
    MemoryArena mesh_storage;
    bool initialized = false;
};