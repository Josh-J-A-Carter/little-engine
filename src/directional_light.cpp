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

    // std::cout << "view space frustum: " << std::endl;
    // print_vec(world_frustum.far_bottom_left);
    // print_vec(world_frustum.far_bottom_right);
    // print_vec(world_frustum.far_top_left);
    // print_vec(world_frustum.far_top_right);
    // print_vec(world_frustum.near_bottom_left);
    // print_vec(world_frustum.near_bottom_right);
    // print_vec(world_frustum.near_top_left);
    // print_vec(world_frustum.near_top_right);

    // Move the frustum to world space
    glm::mat4 camera_view_inv { glm::inverse(camera_view) };
    world_frustum = world_frustum.transform(camera_view_inv);

    // std::cout << "world space frustum: " << std::endl;
    // print_vec(world_frustum.far_bottom_left);
    // print_vec(world_frustum.far_bottom_right);
    // print_vec(world_frustum.far_top_left);
    // print_vec(world_frustum.far_top_right);
    // print_vec(world_frustum.near_bottom_left);
    // print_vec(world_frustum.near_bottom_right);
    // print_vec(world_frustum.near_top_left);
    // print_vec(world_frustum.near_top_right);


    //////////////// This is working correctly
    // std::cout << "test world frustum bounds" << std::endl;
    // bounds test = world_frustum.bounding_box();
    // std::cout << "min: (" << test.min_x << "," << test.min_y << "," << test.min_z << ")" << std::endl;
    // std::cout << "max: (" << test.max_x << "," << test.max_y << "," << test.max_z << ")" << std::endl;

    ///////
    /////// Mistake(s) somewhere below here
    ///////

    // Move the frustum to light space
    glm::mat4 light_view = glm::lookAt(glm::vec3(0), direction, glm::vec3(0, 1, 0));
    frustum light_frustum = world_frustum.transform(light_view);
    
    // Calculate the bounding box of the frustum
    bounds b { light_frustum.bounding_box() };

    std::cout << "light space frustum: " << std::endl;
    print_vec(light_frustum.far_bottom_left);
    print_vec(light_frustum.far_bottom_right);
    print_vec(light_frustum.far_top_left);
    print_vec(light_frustum.far_top_right);
    print_vec(light_frustum.near_bottom_left);
    print_vec(light_frustum.near_bottom_right);
    print_vec(light_frustum.near_top_left);
    print_vec(light_frustum.near_top_right);


    // std::cout << "test light (1) frustum bounds" << std::endl;
    // bounds test = light_frustum.bounding_box();
    // std::cout << "min: (" << test.min_x << "," << test.min_y << "," << test.min_z << ")" << std::endl;
    // std::cout << "max: (" << test.max_x << "," << test.max_y << "," << test.max_z << ")" << std::endl;


    // Calculate the light's position (still in light space, so need to convert to world space)
    glm::vec4 light_pos { (b.min_x + b.max_x) / 2, (b.min_y + b.max_y) / 2, (b.max_z + b.min_z) / 2, 1.0f };
    glm::mat4 light_view_inv { glm::inverse(light_view) };
    light_pos = light_view_inv * light_pos;

    // New light view matrix, centred at light position in world space
    glm::mat4 final_light_view = glm::lookAt(glm::vec3(light_pos), glm::vec3(light_pos) + direction, glm::vec3(0, 1, 0));

    // Transform frustum back into light space, using the new light view matrix, and calculate the orthographic projection
    frustum final_light_frustum = world_frustum.transform(final_light_view);
    bounds final_bounds { final_light_frustum.bounding_box() };

    // std::cout << "test light (2) frustum bounds" << std::endl;
    // bounds test = final_light_frustum.bounding_box();
    // std::cout << "min: (" << test.min_x << "," << test.min_y << "," << test.min_z << ")" << std::endl;
    // std::cout << "max: (" << test.max_x << "," << test.max_y << "," << test.max_z << ")" << std::endl;

    glm::mat4 light_proj { glm::ortho(final_bounds.min_x, final_bounds.max_x, 
                                      final_bounds.min_y, final_bounds.max_y,
                                      final_bounds.min_z, final_bounds.max_z) };
    

    std::cout << "light pos: " << std::endl;
    print_vec(light_pos);
    print_vec(camera->m_pos);

    
    // return glm::ortho(-50.0f, 50.0f,-50.0f, 50.0f,-50.0f, 50.0f) * glm::lookAt(glm::vec3(0), direction, glm::vec3(0, 1, 0));
    // return glm::ortho(-50.0f, 50.0f,-50.0f, 50.0f,-50.0f, 50.0f) * final_light_view;
    return light_proj * final_light_view;
}


// glm::mat4 directional_light::get_shadow_matrix(camera* camera, glm::mat4& camera_view) {
//     // multiply by inverse projection*view matrix to find frustum vertices in world space
//     // transform to light space
//     // same pass, find minimum along each axis
//     glm::mat4 lightSpaceTransform = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(direction), glm::vec3(0.0f, 1.0f, 0.0f));

//     bool firstProcessed = false;
//     glm::vec3 boundingA(std::numeric_limits<float>::infinity());
//     glm::vec3 boundingB(-std::numeric_limits<float>::infinity());

//     // start with <-1 -1 -1> to <1 1 1> cube
//     std::vector<glm::vec4> boundingVertices = {
//         {-1.0f,	-1.0f,	-1.0f,	1.0f},
//         {-1.0f,	-1.0f,	1.0f,	1.0f},
//         {-1.0f,	1.0f,	-1.0f,	1.0f},
//         {-1.0f,	1.0f,	1.0f,	1.0f},
//         {1.0f,	-1.0f,	-1.0f,	1.0f},
//         {1.0f,	-1.0f,	1.0f,	1.0f},
//         {1.0f,	1.0f,	-1.0f,	1.0f},
//         {1.0f,	1.0f,	1.0f,	1.0f}
//     };

//     for (glm::vec4& vert : boundingVertices) {
//         // clip space -> world space
//         vert = camera_view * vert;
//         vert /= vert.w;
//     }

//     for (glm::vec4 vert : boundingVertices) {
//         // clip space -> world space -> light space
//         vert = lightSpaceTransform * vert;

//         // initialize bounds without comparison, only for first transformed vertex
//         if (!firstProcessed) {
//             boundingA = glm::vec3(vert);
//             boundingB = glm::vec3(vert);
//             firstProcessed = true;
//             continue;
//         }

//         // expand bounding box to encompass everything in 3D
//         boundingA.x = std::min(vert.x, boundingA.x);
//         boundingB.x = std::max(vert.x, boundingB.x);
//         boundingA.y = std::min(vert.y, boundingA.y);
//         boundingB.y = std::max(vert.y, boundingB.y);
//         boundingA.z = std::min(vert.z, boundingA.z);
//         boundingB.z = std::max(vert.z, boundingB.z);
//     }

//     // std::vector<glm::vec4> bounds {
//     //     glm::vec4(-50, -1, -50, 1),
//     //     glm::vec4(-50, -1, 50, 1),
//     //     glm::vec4(50, -1, -50, 1),
//     //     glm::vec4(50, -1, 50, 1),

//     //     glm::vec4(-50, 5, -50, 1),
//     //     glm::vec4(-50, 5, 50, 1),
//     //     glm::vec4(50, 5, -50, 1),
//     //     glm::vec4(50, 5, 50, 1)
//     // };

//     // for (glm::vec4 b : bounds) {
//     //     glm::vec4 v = lightSpaceTransform * b;
//     //     boundingA.z = std::min(b.z, boundingA.z);
//     //     boundingB.z = std::max(b.z, boundingB.z);
//     // }


//     // from https://en.wikipedia.org/wiki/Orthographic_projection#Geometry
//     // because I don't trust GLM
//     float l = boundingA.x;
//     float r = boundingB.x;
//     float b = boundingA.y;
//     float t = boundingB.y;
//     float n = boundingA.z;
//     float f = boundingB.z;

//     glm::mat4 ortho = {
//         2.0f / (r - l),	0.0f,			0.0f,			0.0f,
//         0.0f,			2.0f / (t - b),	0.0f,			0.0f,
//         0.0f,			0.0f,			2.0f / (f - n),	0.0f,
//         -(r + l) / (r - l),	-(t + b) / (t - b),	-(f + n) / (f - n),	1.0f
//     };

//     ortho = glm::mat4{
//         1,0,0,0,
//         0,1,0,0,
//         0,0,.5f,0,
//         0,0,.5f,1
//     } * ortho;

//     // project in light space -> world space
//     ortho = ortho * lightSpaceTransform;

//     return  ortho;
// }