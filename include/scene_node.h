#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <string>
#include <vector>

#include "parse_types.h"

struct pipeline;

struct scene_node {
    std::string name { "Object" };
    scene_node_type component_type { scene_node_type::empty };
    void* component { nullptr };

    void (*cmp_load)(scene_node*) { [](scene_node*) {} };
    void (*cmp_run)(scene_node*) { [](scene_node*) {} };
    void (*cmp_render)(scene_node*, pipeline&) { [](scene_node*, pipeline&) {} };
    
    scene_node* parent { nullptr };
    std::vector<scene_node*> children {};

    bool is_valid { true };
    int id { -1 };

    void load();
    void run();
    void render(pipeline& p);
};

template<typename T>
void render(T*, pipeline&) {}

template<typename T>
void load(T*) {}

template<typename T>
void run(T*) {}

#endif