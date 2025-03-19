#include "serialise.h"
#include "scene.h"
#include "scene_node.h"

namespace serial {
    void serialise(std::ostream& os, const scene* sc, const scene_node* _, int indt) {
        serialise(os, sc->root, nullptr, indt);
    }
};
