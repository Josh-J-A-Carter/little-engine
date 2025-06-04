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

bool input() {
    // Update window - it may have been resized
    g_app.update();

    return g_app.quitting();
}

void draw() {
    // Render the scene for this pipeline
    g_app.render();

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
    g_app.create();
    void* context = g_app.window();

    std::optional<error> res = g_app.load_scene("scenes/main.scene");
    if (res.has_value()) std::cout << "Error in opening main scene: " << res.value().message << std::endl;

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

extern "C" void remove_focus() {
    g_app.unfocused = true;
}