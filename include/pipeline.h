#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include "point_light.h"
#include "directional_light.h"
#include "material.h"

#define UNDEFINED_PIPELINE -1
#define STANDARD_PIPELINE 0
#define WATER_PIPELINE 1

struct pipeline {
    public:
        enum uniform {
            UNIFORM_MODEL_MAT,
            UNIFORM_VIEW_MAT,
            UNIFORM_PROJ_MAT,
            UNIFORM_SHADOW0_MAT,
            UNIFORM_SAMPLER_DIFFUSE,
            UNIFORM_SAMPLER_SPECULAR,
            UNIFORM_SAMPLER_SHADOW0,
            UNIFORM_SAMPLER_NOISE,
            UNIFORM_DIR_LIGHTS,
            UNIFORM_POINT_LIGHTS,
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
        
        void initialise(std::vector<shader_src> shaders, int identifier);

        void enable();

        void set_uniform(uniform u, glm::mat4& matrix);
        void set_uniform(uniform u, int input);
        void set_uniform(uniform u, float input);
        void set_uniform(uniform u, std::vector<directional_light*> lights);
        void set_uniform(uniform u, std::vector<point_light*> lights);
        void set_uniform(uniform u, material& material);
        void set_uniform(uniform u, glm::vec3 vector);

        int identifier() { return m_identifier; }

    private:
        void add_shader(GLuint type, std::string file_name);

        void finalise();

        GLint get_uniform_location(uniform u);

        GLuint m_program {};
        std::vector<GLuint> m_temp_shader_handles {};

        int m_identifier { UNDEFINED_PIPELINE };
};

#endif