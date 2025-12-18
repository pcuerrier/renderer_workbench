#include "app.h"
#include "app_internal.h"

RenderQueue AppUpdate()
{
    static bool is_initialized = false;
    MeshHandle triangle_mesh = {};
    if (!is_initialized)
    {
        is_initialized = true;

        float vertices[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left 
        };
        int indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };
        triangle_mesh = Renderer_CreateMesh(vertices, sizeof(vertices), indices, sizeof(indices));
    }
    
    RenderQueue render_queue = {};
    RenderCommand command = {};
    command.mesh = triangle_mesh;
    render_queue.commands = &command;
    render_queue.command_count = 1;
    return render_queue;
}
