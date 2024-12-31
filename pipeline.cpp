#include <string>
#include <iostream>

#include <glad/glad.h>

#include "utilities.h"
#include "pipeline.h"

GLuint pipeline::compile_shader(GLuint type, std::string source_code) {
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

void pipeline::create() {
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

    m_graphics_pipeline_program = program_object;
}

GLuint pipeline::program() {
    return m_graphics_pipeline_program;
}