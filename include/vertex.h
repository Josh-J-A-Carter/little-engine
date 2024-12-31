#ifndef VERTEX_H
#define VERTEX_H

#include <glad/glad.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;

    GLfloat u;
    GLfloat v;

    vertex(glm::vec3 pos, glm::vec2 uv)
        : x { pos.x }
        , y { pos.y }
        , z { pos.z }

        , u { uv.x }
        , v { uv.y }
    {}
};

#endif