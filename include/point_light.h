#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm/vec3.hpp>

#include "object.h"

struct point_light {
    object object;
    
    glm::vec3 color { 1, 1, 1 };
    float ambient_intensity { 0.1 };
    float diffuse_intensity { 1 };
};

#endif