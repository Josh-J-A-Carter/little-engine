#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>

#include "arena.h"
#include "scene_node.h"
#include "transform.h"
#include "mesh.h"
#include "serialise.h"
#include "parse_declarations.h"

struct renderer {
    transform m_transform {};
    mesh m_mesh {};
    std::string filename {};
};

template<>
inline void load<renderer>(renderer* r) {
    r->m_mesh.load(r->filename);
}

REGISTER_PARSE_REF(renderer);

namespace serial {
    inline void serialise(std::ostream& os, const renderer& obj, const scene_node* sc, int indt) {
        serial::serialiser<renderer> sr = { os, obj, sc, indt };

        REPORT(sr, m_transform)
        REPORT(sr, filename)
    }

    template <>
    inline option<renderer*, error> deserialise_ref<renderer>(arena& arena, node* n) {
        renderer* r = arena.allocate<renderer>();

        DESERIALISE_VAL(r, n, m_transform)
        DESERIALISE_VAL(r, n, filename)

        return r;
    }
}

#endif