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

    glm::mat4 transform = glm::mat4(1.0f);
    //static int count = 0;
    //float angle = glm::radians(1.0f * (f32)count++);
    //transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    //transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
    //transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));
    command.transform_matrix = transform;

    render_queue.commands = &command;
    render_queue.command_count = 1;
    return render_queue;
}
