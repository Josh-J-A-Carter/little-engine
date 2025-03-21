#include "serialise.h"
#include "scene.h"
#include "scene_node.h"

namespace serial {
    void serialise_scene(std::ostream& os, const scene* sc) {
        serialise_node(os, sc->root, 0);
    }
};
