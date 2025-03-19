#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <string>
#include <vector>

#include "parse_types.h"

struct scene_node {
    std::string name { "Object" };
    scene_node_type component_type { scene_node_type::empty };
    void* component { nullptr };

    void (*cmp_load)(scene_node*) { [](scene_node* _) {} };
    void (*cmp_run)(scene_node*) { [](scene_node* _) {} };
    void (*cmp_render)(scene_node*) { [](scene_node* _) {} };
    
    scene_node* parent { nullptr };
    std::vector<scene_node*> children {};

    public:
        void load();
        void run();
        void render();
};

template<typename T>
void render(T*) {}

template<typename T>
void load(T*) {}

template<typename T>
void run(T*) {}

#endif