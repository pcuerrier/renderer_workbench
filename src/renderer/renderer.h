#pragma once

#include "core.h"

// Handle types: Just integers or pointers, hiding the real GLuint IDs
struct ShaderHandle { u32 id; };
struct MeshHandle { u32 id; };

struct Vertex {
    float position[3];
    float normal[3];
    float color[3]; // We'll use this for debug colors for now
    float uv[2];    // Texture coordinates (future use)
};

struct RenderCommand {
    MeshHandle mesh;
    ShaderHandle shader;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

struct RenderQueue {
    RenderCommand* commands;
    i64 command_count;
    i64 capacity;
};

// The Interface - Functions implemented by the backend
// System related functions
internal bool Renderer_Init(void* window_handle);
internal void Renderer_Resize(i32 width, i32 height);
internal void Renderer_ClearScreen(f32 r, f32 g, f32 b, f32 a);
internal void Renderer_Present();

// Resource creation functions
internal MeshHandle Renderer_CreateMesh(const Vertex* vertices, int v_count, int* indices, int i_count);
internal ShaderHandle Renderer_CreateShader(const char* vertex_source, const char* fragment_source);

// Rendering functions
internal void Renderer_Draw(RenderCommand* cmd);
