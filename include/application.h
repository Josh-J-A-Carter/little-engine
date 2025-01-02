#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <chrono>

#include "pipeline.h"

#define DEFAULT_WIDTH 960
#define DEFAULT_HEIGHT 720

struct application {
    private:
        int m_window_width { DEFAULT_WIDTH };
        int m_window_height { DEFAULT_HEIGHT };
        SDL_Window* m_window {};
        SDL_GLContext m_openGL_context {};

        std::chrono::high_resolution_clock::time_point m_program_time_start;

        void destroy();

    public:
        ~application() {
            destroy();
        }

        void create();

        float program_time();

        int width();

        int height();

        float aspect();

        SDL_Window* window();

        void update();
};

#endif