#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct transform {
    private:
        glm::vec3 m_pos { 0, 0, 0 };

        glm::vec3 m_rot { 0, 0, 0 };
    
    public:
        transform() {}

        void translate(glm::vec3 delta);

        void rotate(glm::vec3 delta);

        glm::mat4 get_model_matrix() const;
};

#endif