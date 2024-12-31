#include "mesh.h"

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>


glm::mat4 mesh::model_matrix() {
    glm::mat4 model_mat { 1.0f };
    model_mat = glm::translate(model_mat, m_translation);
    model_mat = glm::rotate(model_mat, glm::radians(m_y_rotation), glm::vec3(0, 1, 0));
    return model_mat;
}

void mesh::setup() {
    // VAO (meta data)
    glGenVertexArrays(1, &m_vertex_array_object);
    glBindVertexArray(m_vertex_array_object);

    //// VBO (raw data)
    glGenBuffers(1, &m_vertex_buffer_object);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_object);
    glBufferData(GL_ARRAY_BUFFER, m_vertex_data.size() * sizeof(GLfloat), m_vertex_data.data(), GL_STATIC_DRAW);
    
    size_t stride = sizeof(GLfloat) * (3 + 4);
    GLvoid* pos_offset = (GLvoid*) 0;
    GLvoid* col_offset = (GLvoid*) (3 * sizeof(GLfloat));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, pos_offset);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, col_offset);

    // IBO (indices for each triangle)
    glGenBuffers(1, &m_index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_object);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_data.size() * sizeof(GLuint), m_index_data.data(), GL_STATIC_DRAW);

    // Cleanup
    glBindVertexArray(0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}
