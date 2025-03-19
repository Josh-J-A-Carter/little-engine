#ifndef SCENE_H
#define SCENE_H

#include "arena.h"
#include "serialise.h"
#include "scene_node.h"

#define SCENE_ARENA_SIZE 1024 * 1024


struct scene {
    arena arena { SCENE_ARENA_SIZE };
    scene_node* root { nullptr };
};

namespace serial {
    void serialise(std::ostream& os, const scene* sc, const scene_node* _, int indt);
}

#endif