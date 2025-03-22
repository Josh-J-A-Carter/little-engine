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
// std::vector<mesh> g_meshes {};
pipeline g_pipeline {};
// pipeline g_transparent {};
// camera g_camera {};
// std::vector<point_light> g_point_lights { {}, {} };
// std::vector<directional_light> g_dir_lights { {} };

// std::vector<object> g_opaque_objects {};



void* setup() {
    g_app.create();

    g_pipeline.initialise(
            {
                { GL_VERTEX_SHADER, "shaders/vertex_shader.glsl" },
                { GL_FRAGMENT_SHADER, "shaders/fragment_shader.glsl" }
        }
    );

    // g_transparent.initialise (
    //         {
    //             { GL_VERTEX_SHADER, "shaders/light.vs"},
    //             { GL_FRAGMENT_SHADER, "shaders/light.fs"}
    //     }
    // );

    // g_dir_lights[0].direction = { 1, 0, 0 };
    // g_dir_lights[0].base.color = { 1.0, 0.7, 0.7 };
    // g_dir_lights[0].base.ambient_intensity = 0.2;
    // g_dir_lights[0].base.diffuse_intensity = 1;

    // g_point_lights[0].transform.pos = { 1, 0.5, 0 };
    // g_point_lights[0].attn_linear = 0.25f;
    // g_point_lights[0].base.color = { 0.0, 1.0, 1.0 };
    // g_point_lights[0].base.ambient_intensity = 0.5;
    // g_point_lights[0].base.diffuse_intensity = 1;

    // g_point_lights[1].transform.pos = { 0, 0.5, 0 };
    // g_point_lights[1].base.color = { 0.0, 1.0, 0.0 };
    // g_point_lights[1].base.ambient_intensity = 0.5;
    // g_point_lights[1].base.diffuse_intensity = 1;

    // Set up the camera
    
    return g_app.window();
}

bool input() {
    // Update window - it may have been resized
    g_app.update();

    return g_app.quitting();
}

void draw() {
    // Resetting stuff
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
    
    glViewport(0, 0, g_app.width(), g_app.height());

    float time = g_app.time() / 10;

    // Skybox colour
    glm::vec3 night { 0.2, 0.2, 0.4 };
    glm::vec3 day { 0.4, 0.4, 0.75 };
    glm::vec3 now = day + (night - day) * (-sin(time) * 0.5f + 0.5f);

    // // Directional Light
    // g_dir_lights[0].direction = { cos(time), -sin(time), 0 };
    // g_dir_lights[0].base.diffuse_intensity = 1 * sin(time) * 1.5;
    // g_dir_lights[0].base.specular_intensity = 1 * sin(time) * 1.5;

    // if (g_dir_lights[0].base.diffuse_intensity < 0) {
    //     g_dir_lights[0].base.diffuse_intensity = 0;
    //     g_dir_lights[0].base.specular_intensity = 0;
    // }

    // g_point_lights[0].transform.pos = { cos(time * 20), 0.5, sin(time * 20) };

    glClearColor(now.r, now.g, now.b, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // activate pipeline - this is appropriate location for iterating through multiple if needed
    g_pipeline.enable();

    // Render the scene for this pipeline
    g_app.render(&g_pipeline);

    gl_error_check_barrier

    // Swap buffers
    SDL_GL_SwapWindow(g_app.window());
}

void loop_instance(void* context) {
    bool quit = input();

    draw();
}

void loop() {
    bool quit = false;

    while (!quit) {
        quit = input();
        
        draw();

        int fps_ms = static_cast<int>(g_app.desired_fps * 1000);
        std::this_thread::sleep_for(std::chrono::milliseconds(fps_ms));
   }
}

#ifdef __EMSCRIPTEN__
bool focus_in(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData) {
    g_app.unfocused = false;
    // std::cout << "Focus in" << std::endl;
    return false;
}

bool focus_out(int eventType, const EmscriptenFocusEvent* focusEvent, void* userData) {
    g_app.unfocused = true;
    // std::cout << "Focus out" << std::endl;
    return false;
}
#endif

int main(int argv, char** args)  {
    
    void* context = setup();

    std::optional<error> res = g_app.load_scene("testoutput.txt");
    if (res.has_value()) std::cout << "Error in opening scene: " << res.value().message << std::endl;
    g_app.save_scene("testoutput2.txt");

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