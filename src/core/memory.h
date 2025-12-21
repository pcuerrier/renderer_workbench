#pragma once

#include "core.h"

struct MemoryArena
{
    u64 size;
    u64 used;
    void* base;
    void* curr_offset;
    void* prev_offset;

    // TODO: Add linking to other arenas for more flexibility
};

struct Memory
{
    MemoryArena permanent_storage;
    MemoryArena render_storage;
    bool initialized = false;
};

void InitializeArena(MemoryArena* arena, u64 size, void* base);

void* PushSize(MemoryArena* arena, u64 size)
{
    Assert((arena->used + size) <= arena->size);
    arena->used += size;
    void* result = arena->curr_offset;
    arena->prev_offset = arena->curr_offset;
    arena->curr_offset = (u8*)arena->curr_offset + size;
    return result;
}

void PopSize(MemoryArena* arena, u64 size)
{
    Assert(arena->used >= size);
    arena->used -= size;
    arena->curr_offset = arena->prev_offset;
}