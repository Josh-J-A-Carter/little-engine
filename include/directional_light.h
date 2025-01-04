#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include <glm/vec3.hpp>

#include "object.h"

struct directional_light {
    glm::vec3 direction { 0, 0, -1 };
    glm::vec3 color { 1, 1, 1 };
    float ambient_intensity { 0.1 };
    float diffuse_intensity { 1 };
    float specular_intensity { 1 };
};

#endif