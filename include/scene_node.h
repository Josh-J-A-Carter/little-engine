#ifndef SCENE_NODE_H
#define SCENE_NODE_H

#include <string>
#include <vector>
#include <optional>

#include "parse_types.h"

struct application;
struct pipeline;
struct scene;

struct directional_light;
struct point_light;
struct camera;

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

    void get_directional_lights(std::vector<directional_light*> lights);
    void get_point_lights(std::vector<point_light*> lights);
    std::optional<camera*> get_camera();
};

template<typename T>
void render(application*, scene*, scene_node*, T*, pipeline*) {}

template<typename T>
void load(application*, scene*, scene_node*, T*) {}

template<typename T>
void run(application*, scene*, scene_node*, T*) {}

#endif