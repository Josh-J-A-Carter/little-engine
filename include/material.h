#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>

#include "texture.h"

struct material {
    glm::vec3 ambient_color { 1, 1, 1 };
    glm::vec3 diffuse_color { 1, 1, 1 };
    glm::vec3 specular_color { 1, 1, 1 };

    texture* diffuse_texture { nullptr };
    texture* specular_texture { nullptr };
};

#endif