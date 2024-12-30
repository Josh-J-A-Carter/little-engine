#ifndef CAMERA_H
#define CAMERA_H

#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/rotate_vector.hpp>

struct camera {
    private:
        glm::vec3 m_pos { 0, 0, 0 };
        glm::vec3 m_forward { 0, 0, -1 };
        glm::vec3 m_up { 0, 1, 0 };

        glm::vec2 m_mouse { 0, 0 };
        float m_sensitivity { 0.1 };
        
        float m_fov { 45 };
        float m_aspect { 1 };

    public:
        camera() {}

        camera(glm::vec3 pos, glm::vec3 forward, glm::vec3 up, glm::vec2 mouse)
            : m_pos { pos }
            , m_forward { forward }
            , m_up { up }
            , m_mouse { mouse } {}

        void translate(glm::vec3 delta) {
            m_pos += delta;
        }

        void rotate(glm::vec2 delta) {
            delta *= -1 * m_sensitivity;
            m_mouse += delta;

            m_forward = glm::rotate(m_forward, glm::radians(delta.x), m_up);
        }

        glm::mat4 get_view_matrix() const {
            return glm::lookAt(m_pos, m_pos + m_forward, m_up);
        }

        glm::mat4 get_perspective_matrix() const {
            return glm::perspective(glm::radians(m_fov), m_aspect, 0.1f, 10.0f);
        }

        glm::vec3 up() const {
            return m_up;
        }

        glm::vec3 forward() const {
            return m_forward;
        }

        glm::vec3 left() const {
            return glm::cross(up(), forward());
        }
};

#endif