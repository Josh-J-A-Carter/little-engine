#ifndef CAMERA_H
#define CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct camera {
    private:
        glm::vec3 m_pos { 0, 0, 0 };
        glm::vec3 m_forward { 0, 0, -1 };
        glm::vec3 m_up { 0, 1, 0 };

        glm::vec3 m_facing { m_forward };

        glm::vec2 m_mouse { 0, 0 };
        float m_sensitivity { 0.1 };
        
        float m_near { 0.1f };
        float m_far { 10 };
        float m_fov { 45 };
        float m_aspect { 1 };

    public:
        camera() {}

        // Builder members, for cascading instantiation
        camera& init_pos(glm::vec3 pos) { m_pos = pos ; return *this; }
        camera& init_forward(glm::vec3 forward) { m_forward = forward ; return *this; }
        camera& init_up(glm::vec3 up) { m_up = up ; return *this; }
        camera& init_fov(float fov) { m_fov = fov ; return *this; }
        camera& init_aspect(float aspect) { m_aspect = aspect ; return *this; }
        camera& init_clip(float near, float far) { m_near = near ; m_far = far ; return *this; }

        void translate(glm::vec3 delta);

        void rotate(glm::vec2 delta);

        glm::mat4 get_view_matrix() const;

        glm::mat4 get_perspective_matrix() const;

        glm::vec3 up() const;

        glm::vec3 forward() const;

        glm::vec3 left() const;
};

#endif