#include "scene_node.h"

void scene_node::load() {
    cmp_load(this);

    for (scene_node* child : children) {
        child->load();
    }
}

void scene_node::run() {
    cmp_run(this);

    for (scene_node* child : children) {
        child->run();
    }
}

void scene_node::render() {
    cmp_render(this);

    for (scene_node* child : children) {
        child->render();
    }
}