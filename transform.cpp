#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "transform.h"

glm::mat4 transform::get_model_matrix() const {
    glm::mat4 model_matrix { 1.0f };

    model_matrix = glm::translate(model_matrix, m_pos);

    model_matrix = glm::rotate(model_matrix, glm::radians(m_rot.x), { 1, 0, 0 });
    model_matrix = glm::rotate(model_matrix, glm::radians(m_rot.y), { 0, 1, 0 });
    model_matrix = glm::rotate(model_matrix, glm::radians(m_rot.z), { 0, 0, 1 });

    model_matrix = glm::scale(model_matrix, { 0.1, 0.1, 0.1 });

    return model_matrix;
}

void transform::translate(glm::vec3 delta) {
    m_pos += delta;
}

void transform::rotate(glm::vec3 delta) {
    m_rot += delta;
}