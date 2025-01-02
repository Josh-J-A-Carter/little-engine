#include <string>
#include <iostream>

#include <glad/glad.h>

#include "utilities.h"
#include "pipeline.h"
#include "light.h"
#include "material.h"

void pipeline::initialise(std::vector<shader_src> shaders) {
    m_program = glCreateProgram();

    for (shader_src src : shaders) add_shader(src.type, src.file_name);

    finalise();
}

void pipeline::enable() {
    glUseProgram(m_program);
}

void pipeline::add_shader(GLuint type, std::string file_name) {

    const std::string source { load_from_file(file_name) };

    GLuint shader_object {};

    // Make sure the shader is of an expected type
    if (type == GL_VERTEX_SHADER) shader_object = glCreateShader(type);
    else if (type == GL_FRAGMENT_SHADER) shader_object = glCreateShader(type);
    else {
        std::cerr << "Fatal: Unknown type of shader" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Compile
    const char* source_array = source.c_str();

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

    glAttachShader(m_program, shader_object);

    m_temp_shader_handles.push_back(shader_object);
}

GLint pipeline::get_uniform_location(uniform u) {
    GLint loc { -1 };

    if (u == UNIFORM_MODEL_MAT) loc = glGetUniformLocation(m_program, "u_model_matrix");
    else if (u == UNIFORM_VIEW_MAT) loc = glGetUniformLocation(m_program, "u_view_matrix");
    else if (u == UNIFORM_PROJ_MAT) loc = glGetUniformLocation(m_program, "u_proj_matrix");

    else if (u == UNIFORM_SAMPLER) loc = glGetUniformLocation(m_program, "u_sampler");

    else if (u == UNIFORM_LIGHT__POSITION) loc = glGetUniformLocation(m_program, "u_light.pos");
    else if (u == UNIFORM_LIGHT__COLOR) loc = glGetUniformLocation(m_program, "u_light.color");
    else if (u == UNIFORM_LIGHT__AMBIENT_INTENSITY) loc = glGetUniformLocation(m_program, "u_light.ambient_intensity");
    else if (u == UNIFORM_LIGHT__DIFFUSE_INTENSITY) loc = glGetUniformLocation(m_program, "u_light.diffuse_intensity");

    else if (u == UNIFORM_MATERIAL__AMBIENT_COLOR) loc = glGetUniformLocation(m_program, "u_material.ambient_color");
    else if (u == UNIFORM_MATERIAL__DIFFUSE_COLOR) loc = glGetUniformLocation(m_program, "u_material.diffuse_color");

    else {
        std::cerr << "Error - unhandled uniform variant, with code " << u << std::endl;
        exit(EXIT_FAILURE);
    }

    if (loc == -1) {
        std::cerr << "Warning - unable to find uniform with code " << u << " - it may have been misspelled, or optimised out.\n";
    }

    return loc;
}

void pipeline::set_uniform(uniform u, glm::mat4& matrix) {
    glUniformMatrix4fv(get_uniform_location(u), 1, GL_FALSE, &matrix[0][0]);
}

void pipeline::set_uniform(uniform u, int input) {
    glUniform1i(get_uniform_location(u), input);
}

void pipeline::set_uniform(uniform u, light& light) {
    if (u != UNIFORM_LIGHT) return;

    glUniform3fv(get_uniform_location(UNIFORM_LIGHT__POSITION), 1, &light.pos[0]);
    glUniform3fv(get_uniform_location(UNIFORM_LIGHT__COLOR), 1, &light.color[0]);
    glUniform1f(get_uniform_location(UNIFORM_LIGHT__AMBIENT_INTENSITY), light.ambient_intensity);
    glUniform1f(get_uniform_location(UNIFORM_LIGHT__DIFFUSE_INTENSITY), light.diffuse_intensity);
}

void pipeline::set_uniform(uniform u, material& material) {
    if (u != UNIFORM_MATERIAL) return;

    glUniform3fv(get_uniform_location(UNIFORM_MATERIAL__AMBIENT_COLOR), 1, &material.ambient_color[0]);
    glUniform3fv(get_uniform_location(UNIFORM_MATERIAL__DIFFUSE_COLOR), 1, &material.diffuse_color[0]);
}

void pipeline::finalise() {
    glLinkProgram(m_program);

    // Handle linking errors
    GLint status {};
    glValidateProgram(m_program);
    glGetProgramiv(m_program, GL_VALIDATE_STATUS, &status);

    if (status == GL_FALSE) {
        std::cerr << "Fatal: Graphics pipeline failed to be established; the program object was invalid." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Clean up shader programs
    for (GLuint shader : m_temp_shader_handles) {
        glDetachShader(m_program, shader);
        glDeleteShader(shader);
    }
}
