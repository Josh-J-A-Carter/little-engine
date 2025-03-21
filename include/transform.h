#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "serialise.h"

struct transform {
    glm::vec3 pos { 0, 0, 0 };

    glm::vec3 rot { 0, 0, 0 };

    glm::mat4 get_model_matrix() const;
};

REGISTER_PARSE_REF(transform)

namespace serial {
    inline void serialise(std::ostream& os, const transform& obj, const scene_node* sc, int indt) {
        serial::serialiser<transform> sr = { os, obj, sc, indt };

        REPORT(sr, pos)
        REPORT(sr, rot)
    }

    template <>
    inline option<transform, error> deserialise_val<transform>(arena& arena, node* n) {
        transform tr {};
        transform* tr_ptr { &tr };

        DESERIALISE_VAL(tr_ptr, n, pos)
        DESERIALISE_VAL(tr_ptr, n, rot)

        return tr;
    }

    template <>
    inline option<transform*, error> deserialise_ref<transform>(arena& arena, scene_node* root, node* n) {
        transform* tr = arena.allocate<transform>();

        DESERIALISE_VAL(tr, n, pos)
        DESERIALISE_VAL(tr, n, rot)

        return tr;
    }
}

#endif