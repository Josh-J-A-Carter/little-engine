#ifndef APPLICATION_H
#define APPLICATION_H

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <chrono>
#include <string>
#include <optional>

#include "pipeline.h"
#include "scene.h"
#include "scene_node.h"
#include "utilities.h"
#include "fbo.h"

struct camera;
struct directional_light;
struct point_light;
struct texture;
struct renderer;

#define DEFAULT_WIDTH 1920
#define DEFAULT_HEIGHT 1080
#define DEFAULT_ASPECT DEFAULT_WIDTH / (DEFAULT_HEIGHT * 1.0f)

struct application {
    private:
        int m_window_width { DEFAULT_WIDTH };
        int m_window_height { DEFAULT_HEIGHT };
        int m_logical_width { DEFAULT_WIDTH };
        int m_logical_height { DEFAULT_HEIGHT };
 
        SDL_Window* m_window {};
        SDL_GLContext m_openGL_context {};

        pipeline m_lightpipeline {};
        pipeline m_shadowpipeline {};
        pipeline m_waterpipeline {};

        fbo m_shadowmap {};
        fbo m_reflectionmap {};
        fbo m_refractionmap {};

        texture* m_noise_texture { nullptr };
        texture* m_dudv_texture { nullptr };
        texture* m_normal_texture { nullptr };

        scene* m_scene { nullptr };

        bool m_quitting = false;
        float m_last_frame { calc_program_time() };
        float m_time { calc_program_time() };
        float m_delta_time { 0 };

        std::chrono::high_resolution_clock::time_point m_program_time_start;
        
        void destroy();
        
        float calc_program_time();

        void render_lighting(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights,
                        glm::mat4& view_mat, glm::mat4& proj_mat, glm::mat4& shadow_mat, bool external_setup = false);

        void render_shadows(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights, 
                        glm::mat4& shadow_mat);

        void render_water(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights,
                      glm::mat4& view_mat, glm::mat4& proj_mat, glm::mat4& shadow_mat, renderer* water);
        
    public:
        const float desired_fps = 1 / 60.0f;

    #ifdef __EMSCRIPTEN
        // Used for WebGL
        bool unfocused = true;
    #else
        bool unfocused = false;
    #endif

        ~application() {
            destroy();
        }

        void create();

        inline float time() { return m_time; }

        inline float delta_time() { return m_delta_time; }

        int width();

        int height();

        float aspect();

        SDL_Window* window();

        void update();

        void render();

        std::optional<error> load_scene(std::string);

        std::optional<error> save_scene();

        scene* current_scene();

        void quit() { m_quitting = true; }

        inline bool quitting() { return m_quitting; }
};

#endif