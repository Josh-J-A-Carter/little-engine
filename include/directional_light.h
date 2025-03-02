#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include <glm/vec3.hpp>

#include "light.h"

struct directional_light {
    glm::vec3 direction { 0, 0, -1 };
    light base;
};

#endif