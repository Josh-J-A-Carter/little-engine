#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

struct transform {
    glm::vec3 pos { 0, 0, 0 };

    glm::vec3 rot { 0, 0, 0 };

    glm::mat4 get_model_matrix() const;
};

#endif