#include "app.h"
#include "app_internal.h"

static const char* vertex_shader_source = R"(
    #version 330 core
    // 'layout(location = 0)' matches the index we will use in glVertexAttribPointer later
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;

    out vec3 ourColor; // Output a color to the fragment shader

    // "Uniforms" are global variables we set from the CPU
    uniform mat4 transform;

    void main() {
        // gl_Position is a built-in output variable required by OpenGL
        gl_Position = transform * vec4(aPos, 1.0);
        ourColor = aColor; // Pass the color to the fragment shader
    }
)";

static const char* fragment_shader_source = R"(
    #version 330 core
    out vec4 FragColor; // We define our own output variable
    in vec3 ourColor; // Input from the vertex shader

    void main() {
        // RGBA (Red, Green, Blue, Alpha)
        FragColor = vec4(ourColor, 1.0f);
    }
)";

RenderQueue AppUpdate()
{
    static bool is_initialized = false;
    MeshHandle mesh = {};
    ShaderHandle shader = {};
    if (!is_initialized)
    {
        is_initialized = true;

        Vertex vertices[] = {
            { 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
            { 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
            {-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
            {-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
            { 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
            { 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
            {-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
            {-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f}
        };
        int indices[] = {  // note that we start from 0!
            0, 1, 3,
            1, 2, 3,
            4, 5, 7,
            5, 6, 7,
            0, 1, 4,
            1, 5, 4,
            2, 3, 6,
            3, 7, 6,
            0, 3, 4,
            3, 7, 4,
            1, 2, 5,
            2, 6, 5
        };
        mesh = Renderer_CreateMesh(vertices, sizeof(vertices) / sizeof(Vertex), indices, sizeof(indices) / sizeof(int));
        shader = Renderer_CreateShader(vertex_shader_source, fragment_shader_source);
    }
    
    RenderQueue render_queue = {};
    RenderCommand command = {};
    command.mesh = mesh;
    command.shader = shader;

    static int count = 0;
    float angle = glm::radians(1.0f * (f32)count++);
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.5f, 1.0f, 0.0f));
    glm::mat4 view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    command.transform_matrix = projection * view * model;

    render_queue.commands = &command;
    render_queue.command_count = 1;
    return render_queue;
}
