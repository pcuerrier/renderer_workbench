#include "renderer/renderer.h"

#include "win32_main.h"

#pragma warning(push, 0)
#include "glad/gl.h"
#include "glad/gl.c"
#include "glad/wgl.h"
#include "glad/wgl.c"
#pragma warning(pop)

#include <vector>

// We map our generic MeshHandle to actual OpenGL Vertex Array Objects (VAOs)
struct GLMesh {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int index_count;
};

// Global state or a static array to manage resources
// (In a real engine, you'd have a robust resource manager)
global std::vector<GLMesh> g_meshes;
global std::vector<GLuint> g_shaders;

global HMODULE g_opengl32_module;

namespace renderer
{
internal GLADloadfunc GetProcAddressWGL(const char* procname)
{
    const GLADloadfunc proc = (GLADloadfunc) wglGetProcAddress(procname);
    if (proc)
        return proc;

    return (GLADloadfunc) GetProcAddress(g_opengl32_module, procname);
}

internal bool Init_OpenGL(void* window_handle)
{
    HDC device_context = GetDC((HWND)window_handle);
    // --- STAGE 1: The Dummy Context ---
    // We need a temporary context just to load the modern WGL function pointers.
    PIXELFORMATDESCRIPTOR pfd_dummy = {};
    pfd_dummy.nSize = sizeof(pfd_dummy);
    pfd_dummy.nVersion = 1;
    pfd_dummy.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd_dummy.iPixelType = PFD_TYPE_RGBA;
    pfd_dummy.cColorBits = 32;
    pfd_dummy.cDepthBits = 24;
    pfd_dummy.cStencilBits = 8;

    int pixel_format_dummy = ChoosePixelFormat(device_context, &pfd_dummy);
    SetPixelFormat(device_context, pixel_format_dummy, &pfd_dummy);

    HGLRC gl_context_dummy = wglCreateContext(device_context);
    wglMakeCurrent(device_context, gl_context_dummy);

    g_opengl32_module = LoadLibraryA("opengl32.dll");
    if (!g_opengl32_module)
    {
        printf("Failed to load opengl32.dll!\n");
        return false;
    }

    // --- STAGE 2: Load WGL Extensions ---
    // Now that we have a dummy context, we can load the specific Windows functions
    // that allow us to create a modern profile.
    if (!gladLoadWGL(device_context, (GLADloadfunc)GetProcAddressWGL))
    {
        printf("Failed to load WGL extensions!\n");
        return false;
    }

    // --- STAGE 3: The Real Context ---
    // Define the attributes for a modern OpenGL 3.3+ Core Profile
    int pixel_format_attribs[] =
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        0
    };

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(device_context, pixel_format_attribs, NULL, 1, &pixel_format, &num_formats);

    // Define the context attributes (version 3.3, Core Profile)
    int context_attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 3,
        //WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };

    // Create the modern context
    HGLRC gl_context = wglCreateContextAttribsARB(device_context, 0, context_attribs);

    // Clean up the dummy
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(gl_context_dummy);
    
    // Switch to the new modern context
    wglMakeCurrent(device_context, gl_context);

    // --- STAGE 4: Load OpenGL Functions ---
    // Finally, load all the standard OpenGL functions (glDrawElements, etc.)
    if (!gladLoadGL((GLADloadfunc)GetProcAddressWGL))
    {
        printf("Failed to load OpenGL functions!\n");
        return false;
    }

    printf("OpenGL Initialized: Version %s\n", glGetString(GL_VERSION));
    
    glEnable(GL_DEPTH_TEST);
    return true;
}

internal bool renderer::Init(void* window_handle)
{
    bool init_result = Init_OpenGL(window_handle);
    if (!init_result)
    {
        return false;
    }

    return true;
}

internal void renderer::Resize(i32 width, i32 height)
{
    glViewport(0, 0, width, height);
}

internal void renderer::Present()
{
    glFlush();
}

internal void renderer::ClearScreen(f32 r, f32 g, f32 b, f32 a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

internal MeshHandle renderer::CreateMesh(const Vertex* vertices, int v_count, int* indices, int i_count)
{
    GLMesh mesh = {};

    // Generate the VAO first
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao); // "Start recording configuration..."

    // Generate the VBO and fill it with data
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, v_count * static_cast<int>(sizeof(Vertex)), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_count * static_cast<int>(sizeof(int)), indices, GL_STATIC_DRAW);
    // Layout matches struct Vertex:
    // 0: Position (3 floats)
    // 1: Normal (3 floats)
    // 2: Color (3 floats)
    // 3: UV (2 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO (the VAO "remembers" it)
    glBindVertexArray(0);

    mesh.index_count = i_count;
    g_meshes.push_back(mesh);

    MeshHandle handle = {};
    handle.id = (u32)g_meshes.size() - 1;
    return handle;
}

internal ShaderHandle renderer::CreateShader(const char* vertex_source, const char* fragment_source) {
    // 1. Compile Vertex Shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    
    int  success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    // 2. Compile Fragment Shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    // 3. Link them into a "Program"
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    // 4. Cleanup (We don't need the individual objects once linked)
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    g_shaders.push_back(shader_program);

    ShaderHandle handle = {};
    handle.id = (u32)g_shaders.size() - 1;
    return handle;
}


internal void renderer::Draw(RenderCommand* cmd)
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    if (cmd->mesh.id >= g_meshes.size()) return;
    if (cmd->shader.id >= g_shaders.size()) return;

    GLMesh& mesh = g_meshes[cmd->mesh.id];
    GLuint shader = g_shaders[cmd->shader.id];

    // 2. Setup State
    glUseProgram(shader);

    // 3. Upload Uniforms
    GLint loc_model = glGetUniformLocation(shader, "model");
    glUniformMatrix4fv(loc_model, 1, GL_FALSE, glm::value_ptr(cmd->model));

    GLint loc_view = glGetUniformLocation(shader, "view");
    glUniformMatrix4fv(loc_view, 1, GL_FALSE, glm::value_ptr(cmd->view));

    GLint loc_projection = glGetUniformLocation(shader, "projection");
    glUniformMatrix4fv(loc_projection, 1, GL_FALSE, glm::value_ptr(cmd->projection));

    // 4. Draw
    glBindVertexArray(mesh.vao);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
}

} // namespace renderer