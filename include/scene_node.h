#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <string>
#include <vector>

#include "parse_types.h"

struct application;
struct pipeline;
struct scene;

struct scene_node {
    std::string name { "Object" };
    scene_node_type component_type { scene_node_type::empty };
    void* component { nullptr };

    void (*cmp_load)(application*, scene*, scene_node*) { [](application*, scene*, scene_node*) {} };
    void (*cmp_run)(application*, scene*, scene_node*) { [](application*, scene*, scene_node*) {} };
    void (*cmp_render)(application*, scene*, scene_node*, pipeline*) { [](application*, scene*, scene_node*, pipeline*) {} };
    
    scene_node* parent { nullptr };
    std::vector<scene_node*> children {};

    bool is_valid { true };
    int id { -1 };

    void load(application*, scene*);
    void run(application*, scene*);
    void render(application*, scene*, pipeline* p);
};

template<typename T>
void render(application*, scene*, scene_node*, T*, pipeline*) {}

template<typename T>
void load(application*, scene*, scene_node*, T*) {}

template<typename T>
void run(application*, scene*, scene_node*, T*) {}

#endif