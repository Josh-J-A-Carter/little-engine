#include <iostream>
#include <cstdlib>
#include <SDL2/SDL.h>
#include "glad/glad.h"

// #define NDEBUG

int g_window_height { 480 };
int g_window_width { 640 };
SDL_Window* g_window { nullptr };
SDL_GLContext g_openGL_context { nullptr };

void display_gl_version_info() {
    std::cerr << glGetString(GL_VENDOR) << std::endl;
    std::cerr << glGetString(GL_RENDERER) << std::endl;
    std::cerr << glGetString(GL_VERSION) << std::endl;
    std::cerr << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void setup() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)  {
        std::cerr << "Fatal: Could not initialise SDL; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int screen_width = DM.w;
    int screen_height = DM.h;

    g_window = SDL_CreateWindow("Hello, OpenGL!",
                                (screen_width - g_window_width) / 2, (screen_height - g_window_height) / 2,
                                g_window_width, g_window_height, SDL_WINDOW_OPENGL);

    if (!g_window) {
        std::cerr << "Fatal: Could not create SDL window; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

    g_openGL_context = SDL_GL_CreateContext(g_window);

    if (!g_openGL_context) {
        std::cerr << "Fatal: Could not create OpenGL context; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Fatal: GLAD was unable to be initialised; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

#ifndef NDEBUG
    display_gl_version_info();
#endif
}

bool input() {
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) return true;
    }

    return false;
}

void predraw() {}

void draw() {}

void loop() {
    bool quit = false;

    while (!quit) {
        quit = input();

        predraw();

        draw();

        SDL_GL_SwapWindow(g_window);
    }
}

void cleanup() {
    SDL_DestroyWindow(g_window);

    SDL_Quit();
}

int main(int argv, char** args)  {
    setup();

    loop();

    cleanup();

    return EXIT_SUCCESS;
}