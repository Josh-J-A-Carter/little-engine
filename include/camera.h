#ifndef CAMERA_H
#define CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

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

        void translate(glm::vec3 delta);

        void rotate(glm::vec2 delta);

        glm::mat4 get_view_matrix() const;

        glm::mat4 get_perspective_matrix() const;

        glm::vec3 up() const;

        glm::vec3 forward() const;

        glm::vec3 left() const;
};

#endif