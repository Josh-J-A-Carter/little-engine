#include <ostream>
#include <optional>
#include <Vector>

#include "scene_node.h"
#include "serialise.h"

#include "directional_light.h"
#include "point_light.h"

struct pipeline;
struct application;
struct scene;

void scene_node::load(application* app, scene* scene) {
    cmp_load(app, scene, this);

    for (scene_node* child : children) {
        child->load(app, scene);
    }
}

void scene_node::run(application* app, scene* scene) {
    cmp_run(app, scene, this);

    for (scene_node* child : children) {
        child->run(app, scene);
    }
}

void scene_node::render(application* app, scene* scene, pipeline* p) {
    cmp_render(app, scene, this, p);

    for (scene_node* child : children) {
        child->render(app, scene, p);
    }
}

void scene_node::get_directional_lights(std::vector<directional_light*> lights) {
    if (component_type == scene_node_type::directional_light) lights.push_back(static_cast<directional_light*>(component));
    for (scene_node* child : children) child->get_directional_lights(lights);
}

void scene_node::get_point_lights(std::vector<point_light*> lights) {
    if (component_type == scene_node_type::point_light) lights.push_back(static_cast<point_light*>(component));
    for (scene_node* child : children) child->get_point_lights(lights);
}

std::optional<camera*> scene_node::get_camera() {
    if (component_type == scene_node_type::camera) return static_cast<camera*>(component);
    for (scene_node* child : children) {
        std::optional<camera*> c = child->get_camera();
        if (c.has_value()) return c.value();
    }
    return std::nullopt;
}

/// @brief Serialisation function that is called when the scene_node appears as a reference in a field
/// of another type, as opposed to scene node references in the scene's node hierarchy.
void serial::serialise(std::ostream& os, const scene_node* sc, const scene_node* _, int indt) {
    // References can be either valid or invalid. If valid, this is just the node's ID.
    if (sc->is_valid) os << sc->id;
    else os << "invalid";
}

/// @brief Serialise a list of scene nodes, as the children of another scene_node.
void serial::serialise_node_list(std::ostream& os, const std::vector<scene_node*> list, int indt) {
    if (list.empty()) {
        os << "[]";
        return;
    }
    
    os << "[\n";

    int i_inner = indt + 1;
    for (int i = 0 ; i < list.size() ; i += 1) {
        os << indent(i_inner);
        serialise_node(os, list[i], i_inner);
        os << (i < list.size() - 1 ? ",\n" : "\n");
    }

    os << indent(indt) << "]";   
}

/// @brief Serialise a scene node of the empty variant
void serial::serialise_node_empty(std::ostream& os, const scene_node* sc, int indt) {
    os << "{\n";
    os << indent(indt + 1) << "type: empty,\n";
    os << indent(indt + 1) << "id: " << sc->id << ",\n";
    os << indent(indt + 1) << "name: " << sc->name << ",\n";
    os << indent(indt + 1) << "children: ";
    serialise_node_list(os, sc->children, indt + 2);
    os << "\n" << indent(indt) << "}";
}