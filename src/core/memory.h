#pragma once

#include "core.h"

#include <cstdint>

// Platform specific headers
#if defined(_WIN32)
    #include <windows.h>
#else
    #include <sys/mman.h>
    #include <unistd.h>
    #define INVALID_HANDLE_VALUE -1
#endif

struct VMArena
{
    unsigned char* buffer;    // Start of the massive reserved region
    size_t reserved_size;     // Total address space reserved (e.g., 4GB)
    size_t committed_size;    // How much RAM we've actually asked for
    size_t curr_offset;       // Current allocation bump pointer
};

struct Memory
{
    VMArena permanent_storage;
    VMArena render_storage;
};

namespace memory
{

size_t AlignToPage(size_t size)
{
    size_t page_size = 4096; // TODO: Simplified. Query?
    return (size + page_size - 1) & ~(page_size - 1);
}

void InitVMArena(VMArena* a, size_t max_size)
{
    a->reserved_size = max_size;
    a->curr_offset = 0;
    a->committed_size = 0;

#if defined(_WIN32)
    // MEM_RESERVE: Reserve address space, don't use RAM yet.
    a->buffer = (unsigned char*)VirtualAlloc(0, max_size, MEM_RESERVE, PAGE_NOACCESS);
#else
    // mmap with PROT_NONE: Reserve address space, access triggers crash (segfault)
    // MAP_PRIVATE | MAP_ANON: Private memory, not backed by a file.
    a->buffer = (unsigned char*)mmap(nullptr, max_size, PROT_NONE, 
                                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif

    if (!a->buffer)
    {
        //std::cerr << "Failed to reserve memory!" << std::endl;
        exit(1);
    }
}

void* VMArenaAlloc(VMArena* a, size_t size)
{
    if (a->curr_offset + size > a->reserved_size) {
        return nullptr; // Truly out of address space (very rare with 64-bit)
    }

    // Check if we need to commit more RAM
    if (a->curr_offset + size > a->committed_size)
    {
        // Calculate how much more we need to commit
        size_t needed = (a->curr_offset + size) - a->committed_size;
        size_t commit_amt = AlignToPage(needed); 

#if defined(_WIN32)
        // MEM_COMMIT: Now we back it with RAM.
        void* res = VirtualAlloc(a->buffer + a->committed_size, commit_amt, 
                                 MEM_COMMIT, PAGE_READWRITE);
#else
        // mprotect: Change protection from NONE to READ/WRITE
        int res = mprotect(a->buffer + a->committed_size, commit_amt, 
                           PROT_READ | PROT_WRITE);
        if (res != 0) res = nullptr; // Simple error mapping
#endif

        if (!res)
        {
            return nullptr; // Physical RAM exhausted
        }

        a->committed_size += commit_amt;
    }

    void* ptr = a->buffer + a->curr_offset;
    a->curr_offset += size;
    return ptr;
}

void VMArenaReset(VMArena* a)
{
    a->curr_offset = 0;
}

void VMArenaFree(VMArena* a)
{
#if defined(_WIN32)
    VirtualFree(a->buffer, 0, MEM_RELEASE);
#else
    munmap(a->buffer, a->reserved_size);
#endif
}

} // namespace memory