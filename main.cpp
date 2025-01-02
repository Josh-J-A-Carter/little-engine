#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#include <SDL2/SDL.h>

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include "utilities.h"
#include "application.h"
#include "pipeline.h"
#include "camera.h"
#include "mesh.h"
#include "texture.h"
#include "light.h"


// #define NDEBUG


application g_app {};
std::vector<mesh> g_meshes {};
pipeline g_pipeline {};
camera g_camera {};
light g_light {};

void load_meshes() {
    mesh mesh {};
    mesh.load("./assets/cube.obj");
    mesh.get_transform().translate({ 0, 0, -0.5 });
    g_meshes.push_back(mesh);
}

void setup() {
    g_app.create();

    g_pipeline.initialise(
            {
                { GL_VERTEX_SHADER, "shaders/vertex_shader.glsl" },
                { GL_FRAGMENT_SHADER, "shaders/fragment_shader.glsl" }
        }
    );

    load_meshes();

    // Set up the camera
    g_camera = camera().init_pos({ 0, 0, 0 })
                       .init_aspect({ g_app.aspect() })
                       .init_clip(0.1, 10);

}

bool input() {
    // Update window - it may have been resized
    g_app.update();
    g_camera.init_aspect(g_app.aspect());

    static bool escaped = false;
    static bool just_pressed_escape = false;

    if (!escaped) {
        SDL_WarpMouseInWindow(g_app.window(), g_app.width() / 2, g_app.height() / 2);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) return true;

        if (escaped) continue;

        if (event.type == SDL_MOUSEMOTION) {
            g_camera.rotate({ event.motion.xrel, event.motion.yrel });
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (!escaped) {
        float speed { 0.00025f };

        if (state[SDL_SCANCODE_LCTRL]) speed *= 2;

        if (state[SDL_SCANCODE_W]) g_camera.translate(g_camera.forward() * speed);
        else if (state[SDL_SCANCODE_S]) g_camera.translate(g_camera.forward() * -speed);

        if (state[SDL_SCANCODE_A]) g_camera.translate(g_camera.left() * speed);
        else if (state[SDL_SCANCODE_D]) g_camera.translate(g_camera.left() * -speed);

        if (state[SDL_SCANCODE_SPACE]) g_camera.translate(g_camera.up() * speed);
        else if (state[SDL_SCANCODE_LSHIFT]) g_camera.translate(g_camera.up() * -speed);
    }

    if (state[SDL_SCANCODE_ESCAPE] && !just_pressed_escape) {
        escaped = !escaped;
        just_pressed_escape = true;
    }

    if (just_pressed_escape && !state[SDL_SCANCODE_ESCAPE]) {
        just_pressed_escape = false;
    }

    return false;
}

void draw() {
    // Resetting stuff
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    glViewport(0, 0, g_app.width(), g_app.height());

    glClearColor(0.5f, 0.7f, 0.4f, 1.0f);
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

    // Texture sampler
    g_pipeline.set_uniform(pipeline::UNIFORM_SAMPLER, 0);

    // Light
    g_pipeline.set_uniform(pipeline::UNIFORM_LIGHT, g_light);

    //// -------- Mesh Data ---------

    for (mesh& mesh : g_meshes) {
        mesh.get_transform().rotate({ 0, 0.01, 0 });

        // Model matrix
        glm::mat4 model_mat { mesh.get_transform().get_model_matrix() };
        g_pipeline.set_uniform(pipeline::UNIFORM_MODEL_MAT, model_mat);

        // Ambient material
        g_pipeline.set_uniform(pipeline::UNIFORM_MATERIAL, mesh.get_ambient_material());

        // Draw call
        mesh.render();
    }

    // Swap buffers
    SDL_GL_SwapWindow(g_app.window());
}

void loop() {
    bool quit = false;

    while (!quit) {
        quit = input();

        draw();
    }
}

int main(int argv, char** args)  {
    setup();

    loop();

    return EXIT_SUCCESS;
}