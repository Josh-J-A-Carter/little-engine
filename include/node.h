#ifndef NODE_H
#define NODE_H

#include <string>
#include <vector>

enum node_type {
    EMPTY,
    TRANSFORM,
    CAMERA,
    RENDERER,
    DIRECTIONAL_LIGHT,
    POINT_LIGHT
};

struct node {
    std::string name { "Node" };
    void* component { nullptr };

    void (*fn_render) () { nullptr };
    void (*fn_load) () { nullptr };
    void (*fn_run) () { nullptr };

    void (*fn_script_start) () { nullptr };
    void (*fn_script_run) () { nullptr };

    node* parent { nullptr };
    std::vector<node> children {};
};

#endif