#ifndef SCRIPT_H
#define SCRIPT_H

#include <ostream>
#include <iostream>

#include "utilities.h"
#include "serialise.h"
#include "arena.h"

struct script {
    int a { 0 };
    int b { 100 };

    float c { 400 };

    std::vector<int> r { 1, 2, 3, 4 };
};


REGISTER_PARSE_TYPE(script);

namespace serial {
    inline void serialise(std::ostream& os, const script& obj, int indt = 0) {
        std::cout << "begin serialisation" << std::endl;

        serial::serialiser<script> sr = { os, obj, indt };

        REPORT(sr, a)
        REPORT(sr, b)
        REPORT(sr, c)
        REPORT(sr, r)
    }

    template <>
    inline option<script*, error> deserialise_ref<script>(arena& arena, node* n) {
        script* sc = arena.allocate<script>();

        DESERIALISE_VAL(sc, n, a)
        DESERIALISE_VAL(sc, n, b)
        DESERIALISE_VAL(sc, n, c)
        DESERIALISE_VEC_VAL(sc, n, r)

        return sc;
    }
}

#endif