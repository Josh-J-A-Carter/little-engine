#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <chrono>

#include "pipeline.h"

struct application {
    private:
        int m_window_height { 720 };
        int m_window_width { 960 };
        SDL_Window* m_window {};
        SDL_GLContext m_openGL_context {};

        pipeline m_pipeline;

        std::chrono::high_resolution_clock::time_point m_program_time_start;

    public:
        void create();

        void destroy();

        float program_time();

        int width();

        int height();

        float aspect();

        SDL_Window* window();

        pipeline pipeline();

        void update();
};

#endif