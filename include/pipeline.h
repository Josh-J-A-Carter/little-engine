#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>

#include <glad/glad.h>

struct pipeline {

    void create();

    GLuint program();

    private:
        GLuint compile_shader(GLuint type, std::string source_code);

        GLuint m_graphics_pipeline_program {};
};

#endif