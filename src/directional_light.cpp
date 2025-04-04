#include <glm/vec3.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "light.h"
#include "serialise.h"
#include "application.h"
#include "directional_light.h"

template<>
void run<directional_light>(application* app, scene* scene, scene_node* this_node, directional_light* light) {
    float f = light->frequency;
    if (f > 0) {
        light->direction = { cos(f * app->time()), light->direction[1], sin(f * app->time()) };
    }
}