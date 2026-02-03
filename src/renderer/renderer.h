#pragma once

#include "core.h"
#include "resources/resources_types.h"

// Forward declaration
struct ResourceCatalog;

enum class DrawMode
{
    TRIANGLES,
    LINES,
    LINE_STRIP
};

enum class RenderMode
{
    MESH,
    SPRITE
};

// Handle types: Just integers or pointers, hiding the real GLuint IDs
struct ShaderHandle { u32 id; };
struct MeshHandle { u32 id; };
struct SpriteHandle { u32 id; };

struct Vertex {
    float position[3];
    float normal[3];
    float color[3]; // We'll use this for debug colors for now
    float uv[2];    // Texture coordinates (future use)
};

struct Vertex2D {
    float position[2];
    float color[3];
    float uv[2];
};

struct RenderMeshCommand {
    MeshHandle mesh;
    ShaderHandle shader;
    DrawMode draw_mode;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct RenderSpriteCommand {
    SpriteHandle sprite;
    ShaderHandle shader;
    glm::mat4 model;
    glm::mat4 projection;
};

struct RenderCommand {
    RenderMode mode;
    union {
        RenderMeshCommand mesh_cmd;
        RenderSpriteCommand sprite_cmd;
    };
};

struct RenderQueue {
    RenderCommand* commands;
    u64 command_count;
    u64 capacity;
};

namespace renderer
{

// The Interface - Functions implemented by the backend
// System related functions
internal bool Init(void* window_handle);
internal void Resize(i32 width, i32 height);
internal void ClearScreen(f32 r, f32 g, f32 b, f32 a);
internal void Present();
internal void SetResourceCatalog(ResourceCatalog* catalog);

// Resource creation functions
internal MeshHandle CreateMesh(const Vertex* vertices, int v_count, int* indices, int i_count);
internal ShaderHandle CreateShader(const char* vertex_source, const char* fragment_source);
internal SpriteHandle CreateSprite(ResourceID resource_id, float width, float height);

// Rendering functions
internal void Draw(RenderCommand* cmd);
internal void DrawMesh(RenderMeshCommand* cmd);
internal void DrawSprite(RenderSpriteCommand* cmd);

} // namespace renderer