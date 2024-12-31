#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>


void camera::translate(glm::vec3 delta) {
    m_pos += delta;
}

void camera::rotate(glm::vec2 delta) {
    delta *= -1 * m_sensitivity;
    m_mouse += delta;

    m_forward = glm::rotate(m_forward, glm::radians(delta.x), m_up);
}

glm::mat4 camera::get_view_matrix() const {
    return glm::lookAt(m_pos, m_pos + m_forward, m_up);
}

glm::mat4 camera::get_perspective_matrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspect, 0.1f, 10.0f);
}

glm::vec3 camera::up() const {
    return m_up;
}

glm::vec3 camera::forward() const {
    return m_forward;
}

glm::vec3 camera::left() const {
    return glm::cross(up(), forward());
}
