#include <ostream>

#include "scene_node.h"
#include "serialise.h"

struct pipeline;

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

void scene_node::render(pipeline& p) {
    cmp_render(this, p);

    for (scene_node* child : children) {
        child->render(p);
    }
}

/// @brief Serialisation function that is called when the scene_node appears as a reference in a field
/// of another type, as opposed to scene node references in the scene's node hierarchy
void serial::serialise(std::ostream& os, const scene_node* sc, const scene_node* _, int indt) {
    // References can be either valid or invalid. If valid, this is just the node's ID.
    if (sc->is_valid) os << sc->id;
    else os << "invalid";
}