#include <vector>
#include <string>
#include <iostream>

#include <glad/glad.h>

#include "utilities.h"
#include "pipeline.h"
#include "point_light.h"
#include "directional_light.h"
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

        std::cerr << "Fatal: compilation error in " << shader_type << " shader, " << file_name << std::endl;
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

    else if (u == UNIFORM_SAMPLER_DIFFUSE) loc = glGetUniformLocation(m_program, "u_sampler_diffuse");
    else if (u == UNIFORM_SAMPLER_SPECULAR) loc = glGetUniformLocation(m_program, "u_sampler_specular");

    else if (u == UNIFORM_MATERIAL__AMBIENT_COLOR) loc = glGetUniformLocation(m_program, "u_material.ambient_color");
    else if (u == UNIFORM_MATERIAL__DIFFUSE_COLOR) loc = glGetUniformLocation(m_program, "u_material.diffuse_color");
    else if (u == UNIFORM_MATERIAL__SPECULAR_COLOR) loc = glGetUniformLocation(m_program, "u_material.specular_color");

    else if (u == UNIFORM_TIME) loc = glGetUniformLocation(m_program, "u_time");

    else if (u == UNIFORM_CAMERA) loc = glGetUniformLocation(m_program, "u_camera_pos");

    else {
        std::cerr << "Error - unhandled uniform variant, with code " << u << std::endl;
        exit(EXIT_FAILURE);
    }

    // if (loc == -1) {
    //     std::cerr << "Warning - unable to find uniform with code " << u << " - it may have been misspelled, or optimised out.\n";
    // }

    return loc;
}

void pipeline::set_uniform(uniform u, glm::mat4& matrix) {
    glUniformMatrix4fv(get_uniform_location(u), 1, GL_FALSE, &matrix[0][0]);
}

void pipeline::set_uniform(uniform u, int input) {
    glUniform1i(get_uniform_location(u), input);
}

void pipeline::set_uniform(uniform u, float input) {
    glUniform1f(get_uniform_location(u), input);
}

void pipeline::set_uniform(uniform u, std::vector<directional_light> lights) {
    if (u != UNIFORM_DIR_LIGHTS) return;

    // How many directional lights in the vector?
    glUniform1i(glGetUniformLocation(m_program, "u_num_dir_lights"), lights.size());

    for (int i = 0 ; i < lights.size() ; i += 1) {
        GLint loc { -1 };
        char name[128];

        // Base light fields
        snprintf(name, sizeof(name), "u_dir_lights[%d].base.color", i);
        glUniform3fv(glGetUniformLocation(m_program, name), 1, &lights[i].base.color[0]);
        snprintf(name, sizeof(name), "u_dir_lights[%d].base.ambient_intensity", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].base.ambient_intensity);
        snprintf(name, sizeof(name), "u_dir_lights[%d].base.diffuse_intensity", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].base.diffuse_intensity);
        snprintf(name, sizeof(name), "u_dir_lights[%d].base.specular_intensity", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].base.specular_intensity);

        // Directional light fields
        snprintf(name, sizeof(name), "u_dir_lights[%d].direction", i);
        glUniform3fv(glGetUniformLocation(m_program, name), 1, &lights[i].direction[0]);
    }
}

void pipeline::set_uniform(uniform u, std::vector<point_light> lights) {
    if (u != UNIFORM_POINT_LIGHTS) return;

    // How many points lights in the vector?
    glUniform1i(glGetUniformLocation(m_program, "u_num_point_lights"), lights.size());

    for (int i = 0 ; i < lights.size() ; i += 1) {
        GLint loc { -1 };
        char name[128];

        // Base light fields
        snprintf(name, sizeof(name), "u_point_lights[%d].base.color", i);
        glUniform3fv(glGetUniformLocation(m_program, name), 1, &lights[i].base.color[0]);
        snprintf(name, sizeof(name), "u_point_lights[%d].base.ambient_intensity", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].base.ambient_intensity);
        snprintf(name, sizeof(name), "u_point_lights[%d].base.diffuse_intensity", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].base.diffuse_intensity);
        snprintf(name, sizeof(name), "u_point_lights[%d].base.specular_intensity", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].base.specular_intensity);

        // Point light fields
        snprintf(name, sizeof(name), "u_point_lights[%d].world_pos", i);
        glUniform3fv(glGetUniformLocation(m_program, name), 1, &lights[i].transform.pos[0]);
        snprintf(name, sizeof(name), "u_point_lights[%d].attn_const", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].attn_const);
        snprintf(name, sizeof(name), "u_point_lights[%d].attn_linear", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].attn_linear);
        snprintf(name, sizeof(name), "u_point_lights[%d].attn_exp", i);
        glUniform1f(glGetUniformLocation(m_program, name), lights[i].attn_exp);
    }
}

void pipeline::set_uniform(uniform u, material& material) {
    if (u != UNIFORM_MATERIAL) return;

    glUniform3fv(get_uniform_location(UNIFORM_MATERIAL__AMBIENT_COLOR), 1, &material.ambient_color[0]);
    glUniform3fv(get_uniform_location(UNIFORM_MATERIAL__DIFFUSE_COLOR), 1, &material.diffuse_color[0]);
    glUniform3fv(get_uniform_location(UNIFORM_MATERIAL__SPECULAR_COLOR), 1, &material.specular_color[0]);
}

void pipeline::set_uniform(uniform u, glm::vec3 vector) {
    glUniform3fv(get_uniform_location(u), 1, &vector[0]);
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
