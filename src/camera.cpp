#include "camera.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>


void camera::translate(glm::vec3 delta) {
    m_pos += delta;
}

void camera::rotate(glm::vec2 delta, bool constraint_check) {
    if (constraint_check) delta *= m_sensitivity;
    delta.x *= -1;

    if (abs(m_mouse.y + delta.y) > 80 && constraint_check) {
        delta.y = 0;
    }

    m_mouse += delta;

    m_forward = glm::rotate(m_forward, glm::radians(delta.x), up());
    m_facing = glm::rotate(m_forward, glm::radians(m_mouse.y), left());
}

glm::vec3 camera::position() {
    return m_pos;
}

glm::vec2 camera::rotation() {
    return m_mouse;
}

glm::mat4 camera::get_view_matrix() const {
    return glm::lookAt(m_pos, m_pos + m_facing, m_up);
}

glm::mat4 camera::get_perspective_matrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspect, m_near, m_far);
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
