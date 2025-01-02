#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

struct pipeline {
    public:
        enum uniform {
            UNIFORM_MODEL_MAT,
            UNIFORM_VIEW_MAT,
            UNIFORM_PROJ_MAT,
            UNIFORM_SAMPLER
        };

        void initialise();

        void add_shader(GLuint type, std::string file_name);

        void finalise();

        void enable();

        void set_uniform(uniform u, glm::mat4 matrix);
        void set_uniform(uniform u, int input);

    private:
        GLuint m_program {};
        std::vector<GLuint> m_temp_shader_handles {};

        GLint get_uniform_location(uniform u);
};

#endif