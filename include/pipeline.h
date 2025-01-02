#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "light.h"
#include "material.h"

struct pipeline {
    public:
        enum uniform {
            UNIFORM_MODEL_MAT,
            UNIFORM_VIEW_MAT,
            UNIFORM_PROJ_MAT,
            UNIFORM_SAMPLER,
            UNIFORM_LIGHT,
            UNIFORM_LIGHT__COLOR,
            UNIFORM_LIGHT__AMBIENT_INTENSITY,
            UNIFORM_MATERIAL,
            UNIFORM_MATERIAL__AMBIENT_COLOR
        };

        struct shader_src {
            GLuint type;
            std::string file_name;
        };
        
        void initialise(std::vector<shader_src> shaders);

        void enable();

        void set_uniform(uniform u, glm::mat4& matrix);
        void set_uniform(uniform u, int input);
        void set_uniform(uniform u, light& light);
        void set_uniform(uniform u, material& material);

    private:
        void add_shader(GLuint type, std::string file_name);

        void finalise();

        GLint get_uniform_location(uniform u);

        GLuint m_program {};
        std::vector<GLuint> m_temp_shader_handles {};

};

#endif