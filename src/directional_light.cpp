#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "camera.h"
#include "light.h"
#include "serialise.h"
#include "application.h"
#include "directional_light.h"

#include "utilities.h"

template<>
void run<directional_light>(application* app, scene* scene, scene_node* this_node, directional_light* light) {
    float f = light->frequency;
    if (f > 0) {
        light->direction = { cos(f * app->time()), light->direction[1], sin(f * app->time()) };
    }
}

struct bounds {
    float min_x {};
    float max_x {};

    float min_y {};
    float max_y {};

    float min_z {};
    float max_z {};

    void add(glm::vec3 p) {
        min_x = glm::min(min_x, p.x);
        max_x = glm::max(max_x, p.x);

        min_y = glm::min(min_y, p.y);
        max_y = glm::max(max_y, p.y);

        min_z = glm::min(min_z, p.z);
        max_z = glm::max(max_z, p.z);
    }
};

struct frustum {
    glm::vec4 near_bottom_left {};
    glm::vec4 near_bottom_right {};
    glm::vec4 near_top_left {};
    glm::vec4 near_top_right {};

    glm::vec4 far_bottom_left {};
    glm::vec4 far_bottom_right {};
    glm::vec4 far_top_left {};
    glm::vec4 far_top_right {};

    frustum transform(glm::mat4& m) {
        return {
            near_bottom_left: { m * near_bottom_left },
            near_bottom_right: { m * near_bottom_right },
            near_top_left: { m * near_top_left },
            near_top_right: { m * near_top_right },
            far_bottom_left: { m * far_bottom_left },
            far_bottom_right: { m * far_bottom_right },
            far_top_left: { m * far_top_left },
            far_top_right: { m * far_top_right }
        };
    }

    bounds bounding_box() {
        bounds b {};
        b.add(near_bottom_left);
        b.add(near_bottom_right);
        b.add(near_top_left);
        b.add(near_top_right);
        b.add(far_bottom_left);
        b.add(far_bottom_right);
        b.add(far_top_left);
        b.add(far_top_right);
        return b;
    }
};


// glm::mat4 directional_light::get_shadow_matrix(camera* camera, glm::mat4& camera_view) {
//     float tan_half_fov = tan(glm::radians(camera->m_fov / 2.0f));

//     float near_z = camera->m_near;
//     float near_x = near_z * tan_half_fov;
//     float near_y = near_x / camera->m_aspect;

//     float far_z = camera->m_far;
//     float far_x = far_z * tan_half_fov;
//     float far_y = far_x / camera->m_aspect;

//     // View space frustum corners
//     frustum view_frustum {
//         {-near_x, -near_y, -near_z, 1.0f}, {near_x, -near_y, -near_z, 1.0f},
//         {-near_x,  near_y, -near_z, 1.0f}, {near_x,  near_y, -near_z, 1.0f},
//         {-far_x, -far_y, -far_z, 1.0f},    {far_x, -far_y, -far_z, 1.0f},
//         {-far_x,  far_y, -far_z, 1.0f},    {far_x,  far_y, -far_z, 1.0f}
//     };

//     // Transform to world space
//     glm::mat4 view_inv = glm::inverse(camera_view);
//     frustum world_frustum = view_frustum.transform(view_inv);

//     // Fixed light view matrix (use large offset to simulate far-away sun)
//     glm::vec3 light_target = camera->position(); // Center on camera position
//     glm::vec3 light_pos = light_target + direction;
//     glm::mat4 light_view = glm::lookAt(light_pos, light_target, glm::vec3(0, 1, 0));

//     // Transform frustum to light space
//     frustum light_space_frustum = world_frustum.transform(light_view);
//     bounds light_bounds = light_space_frustum.bounding_box();

//     // Slight z range padding to avoid clipping
//     float z_mult = 10.0f; // optional
//     glm::mat4 light_proj = glm::ortho(
//         light_bounds.min_x, light_bounds.max_x,
//         light_bounds.min_y, light_bounds.max_y,
//         light_bounds.min_z * z_mult, light_bounds.max_z * z_mult
//     );

//     return light_proj * light_view;
// }

glm::mat4 directional_light::get_shadow_matrix(camera* camera, glm::mat4& camera_view) {

    // Get the frustum's view space corners
    float tan_half_fov = tan(glm::radians(camera->m_fov / 2));

    float near_z = camera->m_near;
    float near_x = near_z * tan_half_fov;
    float near_y = near_z * tan_half_fov / camera->m_aspect;

    float far_z = camera->m_far;
    float far_x = far_z * tan_half_fov;
    float far_y = far_z * tan_half_fov / camera->m_aspect;

    frustum world_frustum {
        near_bottom_left: { -near_x, -near_y, -near_z, 1.0f },
        near_bottom_right: { near_x, -near_y, -near_z, 1.0f },
        near_top_left: { -near_x, near_y, -near_z, 1.0f },
        near_top_right: { near_x, near_y, -near_z, 1.0f },
        far_bottom_left: { -far_x, -far_y, -far_z, 1.0f },
        far_bottom_right: { far_x, -far_y, -far_z, 1.0f },
        far_top_left: { -far_x, far_y, -far_z, 1.0f },
        far_top_right: { far_x, far_y, -far_z, 1.0f }
    };

    // Move the frustum to world space
    glm::mat4 camera_view_inv { glm::inverse(camera_view) };
    world_frustum = world_frustum.transform(camera_view_inv);

    // Move the frustum to light space
    glm::mat4 light_view = glm::lookAt(glm::vec3(0), direction, glm::vec3(0, 1, 0));
    frustum light_frustum = world_frustum.transform(light_view);
    
    // Calculate the bounding box of the frustum
    bounds b { light_frustum.bounding_box() };

    // Calculate the light's position (still in light space, so need to convert to world space)
    glm::vec4 light_pos { (b.min_x + b.max_x) / 2, (b.min_y + b.max_y) / 2, (b.max_z + b.min_z) / 2, 1.0f };
    glm::mat4 light_view_inv { glm::inverse(light_view) };
    light_pos = light_view_inv * light_pos;

    // New light view matrix, centred at light position in world space
    glm::mat4 final_light_view = glm::lookAt(glm::vec3(light_pos), glm::vec3(light_pos) + direction, glm::vec3(0, 1, 0));

    // Transform frustum back into light space, using the new light view matrix, and calculate the orthographic projection
    frustum final_light_frustum = world_frustum.transform(final_light_view);
    bounds final_bounds { final_light_frustum.bounding_box() };

    float padding = 1.5f;
    glm::mat4 light_proj { glm::ortho(final_bounds.min_x * padding, final_bounds.max_x * padding, 
                                      final_bounds.min_y * padding, final_bounds.max_y * padding,
                                      final_bounds.min_z * padding, final_bounds.max_z * padding) };
    
    return light_proj * final_light_view;
}

