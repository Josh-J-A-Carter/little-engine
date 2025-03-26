#ifndef DIRECTIONAL_LIGHT_H
#define DIRECTIONAL_LIGHT_H

#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "light.h"
#include "serialise.h"


struct directional_light {
    glm::vec3 direction { 0, 0, -1 };
    light base;
    bool shadow_caster { false };

    glm::mat4 get_shadow_matrix() {
        // projection matrix * view matrix
        return glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, -25.0f, 25.0f)
                * glm::lookAt(glm::vec3(0), direction, glm::vec3(0, 1, 0));
    }
};

REGISTER_PARSE_REF(directional_light);

namespace serial {
    inline void serialise(std::ostream& os, const directional_light& obj, const scene_node* sc, int indt) {
        serial::serialiser<directional_light> sr = { os, obj, sc, indt };

        REPORT(sr, direction)
        REPORT(sr, base)
        REPORT(sr, shadow_caster);
    }

    template <>
    inline option<directional_light*, error> deserialise_ref<directional_light>(arena& arena, scene_node* root, node* n) {
        directional_light* r = arena.allocate<directional_light>();

        DESERIALISE_VAL(r, n, direction)
        DESERIALISE_VAL(r, n, base)
        DESERIALISE_VAL(r, n, shadow_caster)

        return r;
    }
}

#endif