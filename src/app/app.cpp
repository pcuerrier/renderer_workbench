#include "app.h"
#include "app_internal.h"

#include "camera.h"

static const char* vertex_shader_source = R"(
    #version 330 core
    // 'layout(location = 0)' matches the index we will use in glVertexAttribPointer later
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec3 aColor;
    layout (location = 3) in vec2 aTexCoord;

    out vec3 vNormal; // Output to fragment shader
    out vec3 vFragPos; // Output world position (for advanced lighting)
    out vec3 vColor;

    // "Uniforms" are global variables we set from the CPU
    uniform mat4 model; // We need the Model matrix separately now
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        // gl_Position is a built-in output variable required by OpenGL
        gl_Position = projection * view * model * vec4(aPos, 1.0);

        // Transform the normal by the model matrix (rotation)
        // Note: Technically we should use the "Normal Matrix" (inverse transpose) 
        // to handle non-uniform scaling, but for a rotating cube, this works.
        vNormal = aNormal; // Pass the normal to the fragment shader

        vFragPos = vec3(model * vec4(aPos, 1.0)); // Pass the world position to the fragment shader
        vColor = aColor; // Pass the color to the fragment shader
    }
)";

static const char* fragment_shader_source = R"(
    #version 330 core
    out vec4 FragColor; // We define our own output variable

    in vec3 vNormal;
    in vec3 vFragPos;
    in vec3 vColor;

    // Hardcode a light position for now (or pass as uniform)
    vec3 lightPos = vec3(2.0, 2.0, 2.0); 
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    void main() {
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * lightColor;
        
        // 2. Diffuse Light (Directional)
        vec3 norm = normalize(vNormal);
        vec3 lightDir = normalize(lightPos - vFragPos);
        
        // Calculate angle: Dot product
        // max(0.0) ensures we don't get negative light (darker than black)
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // 3. Combine
        vec3 result = (ambient + diffuse) * vColor;
        
        FragColor = vec4(result, 1.0);
    }
)";

RenderQueue AppUpdate(Memory& app_memory, RenderQueue& render_queue, const Input& curr_input, const Input& old_input, float delta_time)
{
    static bool is_initialized = false;
    MeshHandle mesh = {};
    ShaderHandle shader = {};
    static Camera* camera = nullptr;
    float speed = 2.5f;
    float sensitivity = 0.1f;

    if (!is_initialized)
    {
        is_initialized = true;

        Vertex vertices[] = {
            // Front Face (Normal 0, 0, 1)
            { {-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.2f}, {0,0} },
            { { 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.2f}, {1,0} },
            { { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.2f}, {1,1} },
            { {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.5f, 0.2f}, {0,1} },
            // Back Face (Normal 0, 0, -1)
            { {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.5f, 1.0f}, {0,0} },
            { { 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.5f, 1.0f}, {1,0} },
            { { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.5f, 1.0f}, {1,1} },
            { {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.2f, 0.5f, 1.0f}, {0,1} },
            // Top Face (Normal 0, 1, 0)
            { {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f, 0.2f}, {0,0} },
            { { 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f, 0.2f}, {1,0} },
            { { 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f, 0.2f}, {1,1} },
            { {-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.5f, 1.0f, 0.2f}, {0,1} },
            // Bottom Face (Normal 0, -1, 0)
            { {-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.2f, 0.5f}, {0,0} },
            { { 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.2f, 0.5f}, {1,0} },
            { { 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.2f, 0.5f}, {1,1} },
            { {-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.2f, 0.5f}, {0,1} },
            // Left Face (Normal -1, 0, 0)
            { {-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.2f, 1.0f, 0.5f}, {0,0} },
            { {-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.2f, 1.0f, 0.5f}, {1,0} },
            { {-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.2f, 1.0f, 0.5f}, {1,1} },
            { {-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.2f, 1.0f, 0.5f}, {0,1} },
            // Right Face (Normal 1, 0, 0)
            { { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.2f, 1.0f}, {0,0} },
            { { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.2f, 1.0f}, {1,0} },
            { { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.2f, 1.0f}, {1,1} },
            { { 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.5f, 0.2f, 1.0f}, {0,1} }
        };
        int indices[] = {  // note that we start from 0!
            0, 1, 2, 2, 3, 0,         // Front face
            4, 5, 6, 6, 7, 4,         // Back face
            8, 9,10,10,11, 8,         // Top face
           12,13,14,14,15,12,         // Bottom face
           16,17,18,18,19,16,         // Left face
           20,21,22,22,23,20          // Right face
        };
        mesh = Renderer_CreateMesh(vertices, sizeof(vertices) / sizeof(Vertex), indices, sizeof(indices) / sizeof(int));
        shader = Renderer_CreateShader(vertex_shader_source, fragment_shader_source);
        camera = (Camera*)PushSize(&app_memory.permanent_storage, sizeof(Camera));

        Camera_Init(camera, glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    if (curr_input.keyboard.key_w.is_down)
    {
        camera->position += speed * delta_time * camera->forward;
    }
    if (curr_input.keyboard.key_s.is_down)
    {
        camera->position -= speed * delta_time * camera->forward;
    }
    if (curr_input.keyboard.key_a.is_down)
    {
        camera->position -= glm::normalize(glm::cross(camera->forward, camera->up)) * speed * delta_time;
    }
    if (curr_input.keyboard.key_d.is_down)
    {
        camera->position += glm::normalize(glm::cross(camera->forward, camera->up)) * speed * delta_time;
    }

    float dx = (float)(curr_input.mouse.x_pos - old_input.mouse.x_pos);
    float dy = (float)(old_input.mouse.y_pos - curr_input.mouse.y_pos); // Reversed (y-coordinates range from bottom to top)

    // Only update if mouse moved
    /*if (dx != 0 || dy != 0)
    {
        camera->yaw += dx * sensitivity;
        camera->pitch += dy * sensitivity;
        Camera_Update_Vectors(camera);
        // Re-center cursor to keep it from leaving the window (optional but recommended)
        // SetCursorPos(last_mouse_pos.x, last_mouse_pos.y); 
    }*/

    RenderCommand* command = (RenderCommand*)PushSize(&app_memory.render_storage, sizeof(RenderCommand));
    command->mesh = mesh;
    command->shader = shader;

    static int count = 0;
    float angle = glm::radians(1.0f * (f32)count++);
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.5f, 1.0f, 0.5f));
    glm::mat4 view  = Camera_GetViewMatrix(camera);
    //glm::mat4 view  = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    command->model = model;
    command->view = view;
    command->projection = projection;

    render_queue.commands = command;
    render_queue.command_count = 1;
    return render_queue;
}
