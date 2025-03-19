#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <chrono>
#include <thread>

#ifdef __EMSCRIPTEN__
#   include <emscripten.h>
#   include <emscripten/html5.h>
#   include <SDL.h>
#else
#   include <SDL2/SDL.h>
#endif

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "utilities.h"
#include "application.h"
#include "pipeline.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"
#include "point_light.h"
#include "directional_light.h"
#include "object.h"

#include "arena.h"
#include "script.h"
#include "serialise.h"


application g_app {};
std::vector<mesh> g_meshes {};
pipeline g_pipeline {};
pipeline g_transparent {};
camera g_camera {};
std::vector<point_light> g_point_lights { {}, {} };
std::vector<directional_light> g_dir_lights { {} };

std::vector<object> g_opaque_objects {};

const float desired_fps = 1 / 60.0f;

#ifdef __EMSCRIPTEN
// Used for WebGL
bool unfocused = true;
#else
bool unfocused = false;
#endif

float last_frame { 0 };

void load_meshes() {
    mesh cube {};
    cube.load("./assets/cube.obj");
    g_meshes.push_back(cube);

    mesh sphere {};
    sphere.load("./assets/sphere.obj");
    g_meshes.push_back(sphere);

    mesh floor {};
    floor.load("./assets/floor.obj");
    g_meshes.push_back(floor);

    mesh quad {};
    quad.load("./assets/quad.obj");
    g_meshes.push_back(quad);

    g_opaque_objects.push_back({ .transform = { .pos = { 0, 0, -0.3 } }, .mesh = &g_meshes[0] });
    g_opaque_objects.push_back({ .transform = { .pos = { 0.2, 0.3, -0.3 }}, .mesh = &g_meshes[1] });
    g_opaque_objects.push_back({ .transform = { .pos = { 0, -0.2, 0 }}, .mesh = &g_meshes[2] });
}

void* setup() {
    g_app.create();

    g_pipeline.initialise(
            {
                { GL_VERTEX_SHADER, "shaders/vertex_shader.glsl" },
                { GL_FRAGMENT_SHADER, "shaders/fragment_shader.glsl" }
        }
    );

    g_transparent.initialise (
            {
                { GL_VERTEX_SHADER, "shaders/light.vs"},
                { GL_FRAGMENT_SHADER, "shaders/light.fs"}
        }
    );

    load_meshes();

    g_dir_lights[0].direction = { 1, 0, 0 };
    g_dir_lights[0].base.color = { 1.0, 0.7, 0.7 };
    g_dir_lights[0].base.ambient_intensity = 0.2;
    g_dir_lights[0].base.diffuse_intensity = 1;

    g_point_lights[0].transform.pos = { 1, 0.5, 0 };
    g_point_lights[0].attn_linear = 0.25f;
    g_point_lights[0].base.color = { 0.0, 1.0, 1.0 };
    g_point_lights[0].base.ambient_intensity = 0.5;
    g_point_lights[0].base.diffuse_intensity = 1;

    g_point_lights[1].transform.pos = { 0, 0.5, 0 };
    g_point_lights[1].base.color = { 0.0, 1.0, 0.0 };
    g_point_lights[1].base.ambient_intensity = 0.5;
    g_point_lights[1].base.diffuse_intensity = 1;

    // Set up the camera
    g_camera = camera().init_pos({ 0, 0, 0 })
                       .init_aspect({ g_app.aspect() })
                       .init_clip(0.1, 100);
    
    return g_app.window();
}

bool input() {
    float dt = g_app.program_time() - last_frame;

    // Update window - it may have been resized
    g_app.update();
    g_camera.init_aspect(g_app.aspect());

    static bool escaped = false;
    static bool just_pressed_escape = false;

    if (!escaped && !unfocused) {
        SDL_WarpMouseInWindow(g_app.window(), g_app.width() / 2, g_app.height() / 2);
        SDL_SetRelativeMouseMode(SDL_TRUE);
        // std::cout << "capture" << std::endl;
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) return true;

        if (escaped || unfocused) continue;

        if (event.type == SDL_MOUSEMOTION) {
            g_camera.rotate({ event.motion.xrel * dt / desired_fps, event.motion.yrel * dt / desired_fps });
            // std::cout << "rot" << std::endl;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (!escaped && !unfocused) {
        float speed { 0.01f * dt / desired_fps };

        if (state[SDL_SCANCODE_LCTRL]) speed *= 2;

        if (state[SDL_SCANCODE_W]) g_camera.translate(g_camera.forward() * speed);
        else if (state[SDL_SCANCODE_S]) g_camera.translate(g_camera.forward() * -speed);

        if (state[SDL_SCANCODE_A]) g_camera.translate(g_camera.left() * speed);
        else if (state[SDL_SCANCODE_D]) g_camera.translate(g_camera.left() * -speed);

        if (state[SDL_SCANCODE_SPACE]) g_camera.translate(g_camera.up() * speed);
        else if (state[SDL_SCANCODE_LSHIFT]) g_camera.translate(g_camera.up() * -speed);
        // std::cout << "move" << std::endl;
    }

#ifndef __EMSCRIPTEN__
    if (state[SDL_SCANCODE_ESCAPE] && !just_pressed_escape) {
        escaped = !escaped;
        just_pressed_escape = true;
    }

    if (just_pressed_escape && !state[SDL_SCANCODE_ESCAPE]) {
        just_pressed_escape = false;
    }
#else
    if (state[SDL_SCANCODE_ESCAPE]) {
        unfocused = true;
    }
#endif

    return false;
}

void draw() {
    // Resetting stuff
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    
    glViewport(0, 0, g_app.width(), g_app.height());

    float time = g_app.program_time() / 10;

    // Skybox colour
    glm::vec3 night { 0.2, 0.2, 0.4 };
    glm::vec3 day { 0.4, 0.4, 0.75 };
    glm::vec3 now = day + (night - day) * (-sin(time) * 0.5f + 0.5f);

    // Directional Light
    g_dir_lights[0].direction = { cos(time), -sin(time), 0 };
    g_dir_lights[0].base.diffuse_intensity = 1 * sin(time) * 1.5;
    g_dir_lights[0].base.specular_intensity = 1 * sin(time) * 1.5;

    if (g_dir_lights[0].base.diffuse_intensity < 0) {
        g_dir_lights[0].base.diffuse_intensity = 0;
        g_dir_lights[0].base.specular_intensity = 0;
    }

    g_point_lights[0].transform.pos = { cos(time * 20), 0.5, sin(time * 20) };

    glClearColor(now.r, now.g, now.b, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // activate pipeline - this is appropriate location for iterating through multiple if needed
    g_pipeline.enable();

    //// ------ Camera Matrices ------

    // View matrix
    glm::mat4 view_mat { g_camera.get_view_matrix() };
    g_pipeline.set_uniform(pipeline::UNIFORM_VIEW_MAT, view_mat);

    // Perspective matrix
    glm::mat4 proj_mat { g_camera.get_perspective_matrix() };
    g_pipeline.set_uniform(pipeline::UNIFORM_PROJ_MAT, proj_mat);

    //// ------ Other uniforms --------

    // Texture samplers
    g_pipeline.set_uniform(pipeline::UNIFORM_SAMPLER_DIFFUSE, DIFFUSE_TEX_UNIT_INDEX);
    g_pipeline.set_uniform(pipeline::UNIFORM_SAMPLER_SPECULAR, SPECULAR_TEX_UNIT_INDEX);

    // Lights
    g_pipeline.set_uniform(pipeline::UNIFORM_DIR_LIGHTS, g_dir_lights);
    g_pipeline.set_uniform(pipeline::UNIFORM_POINT_LIGHTS, g_point_lights);

    // Miscellaneous
    g_pipeline.set_uniform(pipeline::UNIFORM_CAMERA, g_camera.position());
    g_pipeline.set_uniform(pipeline::UNIFORM_TIME, g_app.program_time());

    //// -------- Mesh Data ---------

    for (object& obj : g_opaque_objects) {
        // obj.transform.rot += glm::vec3 { 0, 0.01, 0 };

        // Model matrix
        glm::mat4 model_mat { obj.transform.get_model_matrix() };
        g_pipeline.set_uniform(pipeline::UNIFORM_MODEL_MAT, model_mat);

        // Ambient material
        g_pipeline.set_uniform(pipeline::UNIFORM_MATERIAL, obj.mesh->get_material());

        // Draw call
        obj.mesh->render();
    }

    gl_error_check_barrier

    // // Multiplicative blending
    // // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // // Additive blending
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    // glEnable(GL_BLEND);

    // g_transparent.enable();

    // // View matrix
    // g_transparent.set_uniform(pipeline::UNIFORM_VIEW_MAT, view_mat);

    // // Perspective matrix
    // g_transparent.set_uniform(pipeline::UNIFORM_PROJ_MAT, proj_mat);

    // glm::vec3 face_cam_dir = glm::normalize(g_camera.position() - g_light.object.transform.pos);
    // glm::vec3 base_face_dir = { 0, 0, -1 };

    // // Rotation around y axis
    // glm::vec3 face_cam_dir_xz = { face_cam_dir.x, 0, face_cam_dir.z };
    // float cos_angle = glm::dot(glm::normalize(face_cam_dir_xz), glm::normalize(base_face_dir));
    // float sin_angle = glm::cross(base_face_dir, face_cam_dir_xz).y;
    // float y_angle_deg = glm::degrees(atan2(sin_angle, cos_angle));

    // // // Rotation around x axis
    // // glm::vec3 face_cam_dir_yz = { 0, face_cam_dir.y, face_cam_dir.z };
    // // cos_angle = glm::dot(glm::normalize(face_cam_dir_yz), glm::normalize(base_face_dir));
    // // sin_angle = glm::cross(base_face_dir, face_cam_dir_yz).x;
    // // float x_angle_deg = glm::degrees(atan2(sin_angle, cos_angle));
    
    // g_light.object.transform.rot = { 0, y_angle_deg, 0 };
    // g_light.object.transform.pos = { 0.6 * cos(g_app.program_time()), 0.4, 0.6 * sin(g_app.program_time()) };

    // // Model matrix
    // glm::mat4 model_mat { g_light.object.transform.get_model_matrix() };
    // g_transparent.set_uniform(pipeline::UNIFORM_MODEL_MAT, model_mat);

    // // Draw call
    // g_light.object.mesh->render();


    // Swap buffers
    SDL_GL_SwapWindow(g_app.window());
}

void loop_instance(void* context) {
    bool quit = input();

    draw();

    last_frame = g_app.program_time();
}

void loop() {
    bool quit = false;

    while (!quit) {
        quit = input();
        
        draw();

        last_frame = g_app.program_time();

        int fps_ms = static_cast<int>(desired_fps * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(fps_ms));
   }
}

#ifdef __EMSCRIPTEN__
bool focus_in(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData) {
    unfocused = false;
    // std::cout << "Focus in" << std::endl;
    return false;
}

bool focus_out(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData) {
    unfocused = true;
    // std::cout << "Focus out" << std::endl;
    return false;
}
#endif

int main(int argv, char** args)  {

    // arena scrap_arena { 512 };
    // script* sc = scrap_arena.allocate<script>();
    // std::ofstream out { "testoutput.txt" };
    // serial::serialise(out, *sc, nullptr, 0);
    // out.close();
    
    option<scene*, error> res = serial::read_scene_from_file("testoutput.txt");
    if (std::holds_alternative<error>(res)) std::cout << std::get<error>(res).message << std::endl;
    else {
        std::cout << "Correctly parsed file as scene" << std::endl;

        std::ofstream out { "testoutput2.txt" };
        scene* new_scene = std::get<scene*>(res);
        serial::serialise(out, new_scene, nullptr, 0);
        out.close();
        std::cout << "Wrote to new file" << std::endl;
    }

    void* context = setup();
    last_frame = g_app.program_time();

#ifdef __EMSCRIPTEN__
    int fps = 0;
    emscripten_set_focusin_callback("#canvas.emscripten", (void*) nullptr, false, focus_in);
    emscripten_set_focusout_callback("#canvas.emscripten", (void*) nullptr, false, focus_out);
    emscripten_set_main_loop_arg(loop_instance, context, fps, true);
#else
    loop();
#endif

    return EXIT_SUCCESS;
}