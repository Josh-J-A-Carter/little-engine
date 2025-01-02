#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>

struct light {
    glm::vec3 color { 1, 1, 1 };
    float ambient_intensity { 1 };
};

#endif