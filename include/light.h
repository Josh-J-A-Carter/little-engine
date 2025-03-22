#ifndef LIGHT_H
#define LIGHT_H

#include <glm/vec3.hpp>

#include "serialise.h"

struct light {
    glm::vec3 color { 1, 1, 1 };
    float ambient_intensity { 0.1 };
    float diffuse_intensity { 1 };
    float specular_intensity { 1 };
};

REGISTER_PARSE_REF(light)

namespace serial {
    inline void serialise(std::ostream& os, const light& obj, const scene_node* sc, int indt) {
        serial::serialiser<light> sr = { os, obj, sc, indt };

        REPORT(sr, color)
        REPORT(sr, ambient_intensity)
        REPORT(sr, diffuse_intensity)
        REPORT(sr, specular_intensity)
    }

    template <>
    inline option<light, error> deserialise_val<light>(arena& arena, node* n) {
        light l {};
        light* l_ptr { &l };

        DESERIALISE_VAL(l_ptr, n, color)
        DESERIALISE_VAL(l_ptr, n, ambient_intensity)
        DESERIALISE_VAL(l_ptr, n, diffuse_intensity)
        DESERIALISE_VAL(l_ptr, n, specular_intensity)

        return l;
    }

    template <>
    inline option<light*, error> deserialise_ref<light>(arena& arena, scene_node* sc, node* n) {
        return { error { "Fields of type light* are disallowed." } };
    }
}

#endif