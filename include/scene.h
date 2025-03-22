#ifndef SCENE_H
#define SCENE_H

#include "arena.h"
#include "serialise.h"
#include "scene_node.h"

#define SCENE_ARENA_SIZE 1024 * 1024

struct pipeline;
struct application;

struct scene {
    arena arena { SCENE_ARENA_SIZE };
    scene_node* root { nullptr };

    inline void load(application* app) {
        root->load(app, this);
    }

    inline void run(application* app) {
        root->run(app, this);
    }

    inline void render(application* app, pipeline* p) {
        root->render(app, this, p);
    }
};

#endif