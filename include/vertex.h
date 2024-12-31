#ifndef VERTEX_H
#define VERTEX_H

#include <glad/glad.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;

    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;

    vertex(glm::vec3 pos, glm::vec4 col)
        : x { pos.x }
        , y { pos.y }
        , z { pos.z }
        , r { col.a }
        , g { col.g }
        , b { col.b }
        , a { col.a }
    {}
};

#endif