#include "renderer/renderer.h"

#include "win32_main.h"

#pragma warning(push, 0)
#include "glad/gl.h"
#include "glad/gl.c"
#include "glad/wgl.h"
#include "glad/wgl.c"
#pragma warning(pop)

// We map our generic MeshHandle to actual OpenGL Vertex Array Objects (VAOs)
struct GLMesh {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
    int index_count;
};

// Global state or a static array to manage resources
// (In a real engine, you'd have a robust resource manager)
global GLMesh g_mesh_storage[1024];

global HMODULE g_opengl32_module;

global GLuint g_shader_program;

//MeshHandle Renderer_CreateMesh(f32* vertices, int v_count, int* indices, int i_count) {
//    // ... extensive OpenGL setup code (glGenBuffers, glBindBuffer, etc.) ...
//    
//    // Return a handle that acts as an ID for this specific mesh
//    MeshHandle handle = { .id = next_free_slot++ };
//    return handle;
//}

const char* vertex_shader_source = R"(
    #version 330 core
    // 'layout(location = 0)' matches the index we will use in glVertexAttribPointer later
    layout (location = 0) in vec3 aPos;

    void main() {
        // gl_Position is a built-in output variable required by OpenGL
        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    }
)";

const char* fragment_shader_source = R"(
    #version 330 core
    out vec4 FragColor; // We define our own output variable

    void main() {
        // RGBA (Red, Green, Blue, Alpha)
        FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    }
)";

internal GLADloadfunc GetProcAddressWGL(const char* procname)
{
    const GLADloadfunc proc = (GLADloadfunc) wglGetProcAddress(procname);
    if (proc)
        return proc;

    return (GLADloadfunc) GetProcAddress(g_opengl32_module, procname);
}

GLuint CreateShaderProgram(const char* vertex_source, const char* fragment_source) {
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

    return shader_program;
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
    //char debug_text_buffer[256] = {};
    //sprintf_s(debug_text_buffer, sizeof(debug_text_buffer), "OpenGL Version: %s\n", glGetString(GL_VERSION));
    //OutputDebugStringA(debug_text_buffer);
    return true;
}

internal bool Renderer_Init(void* window_handle)
{
    bool init_result = Init_OpenGL(window_handle);
    if (!init_result)
    {
        return false;
    }

    g_shader_program = CreateShaderProgram(vertex_shader_source, fragment_shader_source);

    return true;
}

internal void Renderer_Resize(i32 width, i32 height)
{
    glViewport(0, 0, width, height);
}

internal void Renderer_ClearScreen(f32 r, f32 g, f32 b, f32 a)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(r, g, b, a);
}

internal MeshHandle Renderer_CreateMesh(f32* vertices, int v_count, int* indices, int i_count)
{
    GLuint VBO, VAO, EBO;
    // Generate the VAO first
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); // "Start recording configuration..."

    // Generate the VBO and fill it with data
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v_count, vertices, GL_STATIC_DRAW);

    // --- 3. Define the Layout (The Recipe) ---
    // "Attribute 0 is 3 floats, tightly packed, starting at offset 0"
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_count, indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, i_count, GL_UNSIGNED_INT, 0);

    // Unbind VBO/VAO to be safe (optional but good practice)
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0);

    GLMesh mesh = {};
    mesh.vao = VAO;
    mesh.vbo = VBO;
    mesh.ebo = EBO;
    mesh.index_count = i_count;
    g_mesh_storage[0] = mesh;

    MeshHandle handle = {};
    handle.id = 0;
    return handle;
}

internal void Renderer_Draw(RenderCommand* cmd)
{
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
    GLMesh mesh = g_mesh_storage[cmd->mesh.id];
    glUseProgram(g_shader_program);
    glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
}

internal void Renderer_Present()
{
    glFlush();
}
