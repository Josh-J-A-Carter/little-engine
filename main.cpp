#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include "glad/glad.h"

// #define NDEBUG

int g_window_height { 480 };
int g_window_width { 640 };
SDL_Window* g_window {};
SDL_GLContext g_openGL_context {};

GLuint g_vertex_array_object {};
GLuint g_vertex_buffer_object {};

GLuint g_graphics_pipeline_program {};

std::string load_from_file(const std::string& file_name) {
    std::string result {};

    std::string line {};

    std::ifstream file { file_name };

    // Make sure the file actually exists
    if (file.is_open() == false) {
        std::cerr << "Fatal: unable to find file \"" << file_name << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }

    while (std::getline(file, line)) {
        result.append(line)
              .append("\n");
    }

    file.close();

    return result;
}

void display_gl_version_info() {
    std::cerr << glGetString(GL_VENDOR) << std::endl;
    std::cerr << glGetString(GL_RENDERER) << std::endl;
    std::cerr << glGetString(GL_VERSION) << std::endl;
    std::cerr << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)  {
        std::cerr << "Fatal: Could not initialise SDL2; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int screen_width = DM.w;
    int screen_height = DM.h;

    g_window = SDL_CreateWindow("Hello, OpenGL!",
                                (screen_width - g_window_width) / 2, (screen_height - g_window_height) / 2,
                                g_window_width, g_window_height, SDL_WINDOW_OPENGL);

    if (!g_window) {
        std::cerr << "Fatal: Could not create SDL window; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

    g_openGL_context = SDL_GL_CreateContext(g_window);

    if (!g_openGL_context) {
        std::cerr << "Fatal: Could not create OpenGL context; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Fatal: GLAD was unable to be initialised; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

#ifndef NDEBUG
    display_gl_version_info();
#endif
}

void vertex_specification() {
    const std::vector<GLfloat> vertex_data {
        //v1
        -0.8f, -0.8f, 0.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        //v2
        0.8f, -0.8f, 0.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        //v3
        0.0f, 0.8f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    // VAO (meta data)
    glGenVertexArrays(1, &g_vertex_array_object);
    glBindVertexArray(g_vertex_array_object);

    //// VBO (raw data)
    glGenBuffers(1, &g_vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(GLfloat), vertex_data.data(), GL_STATIC_DRAW);
    
    size_t stride = sizeof(GLfloat) * (3 + 4);
    GLvoid* pos_offset = (GLvoid*) 0;
    GLvoid* col_offset = (GLvoid*) (3 * sizeof(GLfloat));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, pos_offset);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, col_offset);

    // Cleanup
    glBindVertexArray(0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

GLuint compile_shader(GLuint type, std::string source_code) {
    GLuint shader_object {};

    // Make sure the shader is of an expected type
    if (type == GL_VERTEX_SHADER) {
        shader_object = glCreateShader(type);
    }

    else if (type == GL_FRAGMENT_SHADER) {
        shader_object = glCreateShader(type);
    }

    else {
        std::cerr << "Fatal: Unknown type of shader" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Compile
    const char* source_array = source_code.c_str();

    glShaderSource(shader_object, 1, &source_array, nullptr);
    glCompileShader(shader_object);

    // Handle compilation errors
    GLint status {};
    glGetShaderiv(shader_object, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) {
        GLint length;
        glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &length);
        char* error_msg = new char[length];
        glGetShaderInfoLog(shader_object, length, &length, error_msg);
        
        std::string shader_type = type == GL_VERTEX_SHADER ? "vertex" : "fragment";

        std::cerr << "Fatal: compilation error in " << shader_type << " shader." << std::endl;
        std::cerr << "Message:   " << error_msg << std::endl;
        exit(EXIT_FAILURE);
    }

    return shader_object;
}

void create_graphics_pipeline() {
    GLuint program_object = glCreateProgram();

    // Compile
    const std::string g_vertex_shader { load_from_file("shaders/vertex_shader.glsl") };
    const std::string g_fragment_shader { load_from_file("shaders/fragment_shader.glsl") };

    GLuint compiled_vertex_shader = compile_shader(GL_VERTEX_SHADER, g_vertex_shader);
    GLuint compiled_fragment_shader = compile_shader(GL_FRAGMENT_SHADER, g_fragment_shader);

    // Link
    glAttachShader(program_object, compiled_vertex_shader);
    glAttachShader(program_object, compiled_fragment_shader);
    glLinkProgram(program_object);

    // Handle linking errors
    GLint status {};
    glValidateProgram(program_object);
    glGetProgramiv(program_object, GL_VALIDATE_STATUS, &status);

    if (status == GL_FALSE) {
        std::cerr << "Fatal: Graphics pipeline failed to be established; the program object was invalid." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Clean up shader programs
    glDetachShader(program_object, compiled_vertex_shader);
    glDetachShader(program_object, compiled_fragment_shader);
    glDeleteShader(compiled_vertex_shader);
    glDeleteShader(compiled_fragment_shader);

    g_graphics_pipeline_program = program_object;
}

bool input() {
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) return true;
    }

    return false;
}

void predraw() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, g_window_width, g_window_height);

    glClearColor(0.5f, 0.7f, 0.4f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glUseProgram(g_graphics_pipeline_program);
}

void draw() {
    glBindVertexArray(g_vertex_array_object);
    glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer_object);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Cleanup - only actually necessary if we have multiple pipelines
    glUseProgram(0);
}

void loop() {
    bool quit = false;

    while (!quit) {
        quit = input();

        predraw();

        draw();

        SDL_GL_SwapWindow(g_window);
    }
}

void cleanup() {
    SDL_DestroyWindow(g_window);

    SDL_Quit();
}

int main(int argv, char** args)  {
    setup();

    create_graphics_pipeline();

    vertex_specification();

    loop();

    cleanup();

    return EXIT_SUCCESS;
}