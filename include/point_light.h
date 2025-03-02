#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm/vec3.hpp>

#include "transform.h"
#include "light.h"

struct point_light {
    transform transform;
    light base;
    float attn_const { 1.0f };
    float attn_linear { 1.0f };
    float attn_exp { 0.0f };
};

#endif