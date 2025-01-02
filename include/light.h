#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>

struct light {
    glm::vec3 pos { 0, 0, 0 };
    glm::vec3 color { 1, 1, 1 };
    float ambient_intensity { 0.1 };
    float diffuse_intensity { 1 };
};

#endif