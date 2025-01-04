#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "point_light.h"
#include "directional_light.h"
#include "material.h"

struct pipeline {
    public:
        enum uniform {
            UNIFORM_MODEL_MAT,
            UNIFORM_VIEW_MAT,
            UNIFORM_PROJ_MAT,
            UNIFORM_SAMPLER_DIFFUSE,
            UNIFORM_SAMPLER_SPECULAR,
            UNIFORM_DIR_LIGHT,
            UNIFORM_DIR_LIGHT__DIRECTION,
            UNIFORM_DIR_LIGHT__COLOR,
            UNIFORM_DIR_LIGHT__AMBIENT_INTENSITY,
            UNIFORM_DIR_LIGHT__DIFFUSE_INTENSITY,
            UNIFORM_DIR_LIGHT__SPECULAR_INTENSITY,
            UNIFORM_MATERIAL,
            UNIFORM_MATERIAL__AMBIENT_COLOR,
            UNIFORM_MATERIAL__DIFFUSE_COLOR,
            UNIFORM_MATERIAL__SPECULAR_COLOR,
            UNIFORM_TIME,
            UNIFORM_CAMERA
        };

        struct shader_src {
            GLuint type;
            std::string file_name;
        };
        
        void initialise(std::vector<shader_src> shaders);

        void enable();

        void set_uniform(uniform u, glm::mat4& matrix);
        void set_uniform(uniform u, int input);
        void set_uniform(uniform u, float input);
        // void set_uniform(uniform u, point_light& light);
        void set_uniform(uniform u, directional_light& light);
        void set_uniform(uniform u, material& material);
        void set_uniform(uniform u, glm::vec3 vector);

    private:
        void add_shader(GLuint type, std::string file_name);

        void finalise();

        GLint get_uniform_location(uniform u);

        GLuint m_program {};
        std::vector<GLuint> m_temp_shader_handles {};

};

#endif