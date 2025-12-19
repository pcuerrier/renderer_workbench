#pragma once

#include "core.h"

// Handle types: Just integers or pointers, hiding the real GLuint IDs
struct TextureHandle { u32 id; };
struct MeshHandle { u32 id; };

struct RenderCommand {
    MeshHandle mesh;
    TextureHandle texture;
    glm::mat4 transform_matrix;
};

struct RenderQueue {
    RenderCommand* commands;
    i64 command_count;
    i64 capacity;
};

// The Interface - Functions implemented by the backend
internal bool Renderer_Init(void* window_handle);
internal void Renderer_Resize(i32 width, i32 height);
internal void Renderer_ClearScreen(f32 r, f32 g, f32 b, f32 a);
internal MeshHandle Renderer_CreateMesh(f32* vertices, int v_count, int* indices, int i_count);
internal void Renderer_Draw(RenderCommand* cmd);
internal void Renderer_Present();
