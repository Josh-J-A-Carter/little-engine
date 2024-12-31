#ifndef MESH_H
#define MESH_H

#include <vector>

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct mesh {
    GLuint m_vertex_array_object {};
    GLuint m_vertex_buffer_object {};
    GLuint m_index_buffer_object {};

    std::vector<GLfloat> m_vertex_data {};
    std::vector<GLuint> m_index_data {};

    glm::vec3 m_translation;

    float m_y_rotation;

    mesh(std::vector<GLfloat> vertex_data, std::vector<GLuint> index_data)
        : m_vertex_data { vertex_data }
        , m_index_data { index_data } {
        setup();
    }

    glm::mat4 model_matrix();

    private:
        void setup();
};

#endif