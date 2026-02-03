#pragma once

#include "core.h"
#include "core/memory.h"

#include "resources/resources_types.h"

enum class ResourceType
{
    RES_UNKNOWN = 0,
    RES_SPRITE,
    RES_AUDIO,
    RES_SCRIPT,
    RES_LEVEL_DATA
};

struct Resource
{
    void* rawBuffer;      // Pointer to the start of memory
    size_t size;          // Size of the buffer in bytes
    ResourceType type;    // Type tag for safe casting later
    ResourceID id;        // Unique ID for this resource
};

// We use a forward declaration to hide the implementation details (std::map, etc.)
// This is known as an "Opaque Pointer" or "Pimpl" idiom.
struct ResourceCatalog;

// --- API Functions ---

// Lifecycle
ResourceCatalog* Catalog_Create();
void Catalog_Destroy(ResourceCatalog* catalog);

// Operations
// Returns true if load was successful
ResourceID Catalog_Load(ResourceCatalog* catalog, const char* filepath, ResourceType type);

// Unloads a specific resource to free memory
//void Catalog_Unload(ResourceCatalog* catalog, const char* filepath);

// The core retrieval function
Resource* Catalog_Get(ResourceCatalog* catalog, ResourceID id);