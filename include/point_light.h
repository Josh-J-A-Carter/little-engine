#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm/vec3.hpp>

#include "transform.h"
#include "light.h"
#include "serialise.h"

struct point_light {
    transform transform;
    light base;
    float attn_const { 1.0f };
    float attn_linear { 1.0f };
    float attn_exp { 0.0f };
};

REGISTER_PARSE_REF(point_light);

namespace serial {
    inline void serialise(std::ostream& os, const point_light& obj, const scene_node* sc, int indt) {
        serial::serialiser<point_light> sr = { os, obj, sc, indt };

        REPORT(sr, transform)
        REPORT(sr, base)
        REPORT(sr, attn_const)
        REPORT(sr, attn_linear)
        REPORT(sr, attn_exp)
    }

    template <>
    inline option<point_light*, error> deserialise_ref<point_light>(arena& arena, scene_node* root, node* n) {
        point_light* r = arena.allocate<point_light>();

        DESERIALISE_VAL(r, n, transform)
        DESERIALISE_VAL(r, n, base)
        DESERIALISE_VAL(r, n, attn_const)
        DESERIALISE_VAL(r, n, attn_linear)
        DESERIALISE_VAL(r, n, attn_exp)

        return r;
    }
}

#endif