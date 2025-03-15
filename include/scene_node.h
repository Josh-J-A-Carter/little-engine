#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <string>
#include <vector>

#include "serialise.h"
#include "parse_types.h"

struct scene_node {
    std::string name { "Object" };
    scene_node_type component_type { scene_node_type::empty };
    void* component { nullptr };

    void (*fn_render) () { nullptr };
    void (*fn_load) () { nullptr };
    void (*fn_run) () { nullptr };

    void (*fn_script_start) () { nullptr };
    void (*fn_script_run) () { nullptr };

    scene_node* parent { nullptr };
    std::vector<scene_node*> children {};
};

#endif