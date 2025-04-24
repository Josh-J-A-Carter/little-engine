#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <optional>
#include <string>

#include "arena.h"
#include "serialise.h"
#include "scene_node.h"

#define SCENE_ARENA_SIZE 1024 * 1024

struct pipeline;
struct application;

struct directional_light;
struct point_light;
struct camera;
struct renderer;

struct scene {
    arena arena { SCENE_ARENA_SIZE };
    scene_node* root { nullptr };

    std::string filename {};

    inline void load(application* app) {
        root->load(app, this);
    }

    inline void run(application* app) {
        root->run(app, this);
    }

    inline void render(application* app, pipeline* p) {
        root->render(app, this, p);
    }

    inline std::vector<directional_light*> get_directional_lights() {
        std::vector<directional_light*> lights {};
        root->get_directional_lights(lights);
        return lights;
    }

    inline std::vector<point_light*> get_point_lights() {
        std::vector<point_light*> lights {};
        root->get_point_lights(lights);
        return lights;
    }

    inline std::optional<camera*> get_camera() {
        return root->get_camera();
    }

    inline std::optional<renderer*> get_water_renderer() {
        return root->get_water_renderer();
    }
};

#endif