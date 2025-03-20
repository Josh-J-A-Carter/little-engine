#include <iostream>
#include <optional>

#include "utilities.h"
#include "application.h"
#include "pipeline.h"
#include "scene.h"
#include "serialise.h"

void application::create() {
    m_program_time_start = std::chrono::high_resolution_clock::now();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)  {
        std::cerr << "Fatal: Could not initialise SDL2; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }
#ifdef __EMSCRIPTEN__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    int screen_width = DM.w;
    int screen_height = DM.h;

    m_window_width = screen_width;
    m_window_height = screen_height * DEFAULT_ASPECT;

    m_window = SDL_CreateWindow("OpenGL Renderer",
                                (screen_width - m_window_width) / 2, (screen_height - m_window_height) / 2,
                                m_window_width, m_window_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED);

    SDL_Renderer* renderer = SDL_GetRenderer(m_window);
    SDL_RenderSetLogicalSize(renderer, m_logical_width, m_logical_height);

    if (!m_window) {
        std::cerr << "Fatal: Could not create SDL window; aborting program." << std::endl;
        exit(EXIT_FAILURE);
    }

    m_openGL_context = SDL_GL_CreateContext(m_window);

    if (!m_openGL_context) {
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

void application::destroy() {
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}

float application::program_time() {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> program_time = now - m_program_time_start;
    return program_time.count();
}

int application::width() {
    return m_window_width;
}

int application::height() {
    return m_window_height;
}

float application::aspect() {
    return static_cast<float>(m_window_width) / m_window_height;
}

SDL_Window* application::window() {
    return m_window;
}

void application::update() {
    int width { m_window_width };
    int height { m_window_height };

    SDL_GetWindowSize(m_window, &width, &height);

    m_window_width = width;
    m_window_height = height;

    m_scene->run();
}

std::optional<error> application::load_scene(std::string filename) {
    option<scene*, error> res = serial::read_scene_from_file(filename);
    if (std::holds_alternative<error>(res)) return std::get<error>(res);

    m_scene = std::get<scene*>(res);
    return std::nullopt;
}

void application::render(pipeline& p) {
    if (m_scene) m_scene->render(p);
}
