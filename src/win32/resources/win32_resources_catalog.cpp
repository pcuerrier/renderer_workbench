#include "resources/resources_catalog.h"
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>

struct ResourceCatalog
{
    // Key is now uint32_t (ResourceID)
    std::unordered_map<ResourceID, Resource> registry;
};

// ... Internal_ReadFileToBuffer implementation remains the same ...
static void* Internal_ReadFileToBuffer(const char* path, size_t* outSize)
{
    FILE* f;
    errno_t err = fopen_s(&f, path, "rb");
    if (err != 0) return nullptr;
    fseek(f, 0, SEEK_END);
    *outSize = static_cast<size_t>(ftell(f));
    fseek(f, 0, SEEK_SET);
    void* buffer = malloc(*outSize); 
    if (buffer) fread(buffer, 1, *outSize, f);
    fclose(f);
    return buffer;
}

ResourceCatalog* Catalog_Create()
{
    return new ResourceCatalog();
}

void Catalog_Destroy(ResourceCatalog* catalog)
{
    if (!catalog) return;
    for (auto& pair : catalog->registry)
    {
        if (pair.second.rawBuffer) free(pair.second.rawBuffer);
    }
    delete catalog;
}

ResourceID Catalog_Load(ResourceCatalog* catalog, const char* filepath, ResourceType type)
{
    if (!catalog || !filepath) return INVALID_RESOURCE_ID;

    // 1. Generate ID from path
    ResourceID id = HashString(filepath);

    // 2. Check if exists
    if (catalog->registry.find(id) != catalog->registry.end())
    {
        return id; // Already loaded, return existing ID
    }

    // 3. Load File
    size_t size = 0;
    void* data = Internal_ReadFileToBuffer(filepath, &size);
    if (!data) return INVALID_RESOURCE_ID;

    Resource res;
    res.rawBuffer = data;
    res.size = size;
    res.type = type;
    res.id = id;

    // 4. Store using ID
    catalog->registry[id] = res;
    
    return id;
}

Resource* Catalog_Get(ResourceCatalog* catalog, ResourceID id)
{
    if (!catalog) return nullptr;
    
    auto it = catalog->registry.find(id);
    if (it != catalog->registry.end())
    {
        return &it->second;
    }
    return nullptr;
}