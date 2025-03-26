#include <iostream>
#include <optional>
#include <ostream>
#include <glm/ext/matrix_clip_space.hpp>

#include "utilities.h"
#include "application.h"
#include "pipeline.h"
#include "scene.h"
#include "camera.h"
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

    // m_window_width = screen_width;
    // m_window_height = screen_width * DEFAULT_ASPECT;

    m_window = SDL_CreateWindow("Little Engine",
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

    // Set up pipeline
    m_lightpipeline.initialise({
            { GL_VERTEX_SHADER, "shaders/phong.vs" },
            { GL_FRAGMENT_SHADER, "shaders/phong.fs" }
        });
    
    m_shadowpipeline.initialise({
            { GL_VERTEX_SHADER, "shaders/shadow.vs" },
            { GL_FRAGMENT_SHADER, "shaders/shadow.fs" }
        });

    // Set up shadow map
    m_shadowmap.initialise();
}

void application::destroy() {
    SDL_DestroyWindow(m_window);

    SDL_Quit();
}

float application::calc_program_time() {
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
    m_last_frame = m_time;
    m_time = calc_program_time();
    m_delta_time = m_last_frame - m_time;

    int width { m_window_width };
    int height { m_window_height };

    SDL_GetWindowSize(m_window, &width, &height);

    m_window_width = width;
    m_window_height = height;

    if (m_scene) m_scene->run(this);
}

scene* application::current_scene() {
    return m_scene;
}

std::optional<error> application::load_scene(std::string filename) {
    option<scene*, error> res = serial::read_scene_from_file(filename);
    if (std::holds_alternative<error>(res)) return std::get<error>(res);

    m_scene = std::get<scene*>(res);
    m_scene->load(this);
    return std::nullopt;
}

std::optional<error> application::save_scene(std::string filename) {
    if (m_scene == nullptr) return { error { "Attempted to saved scene, but no scene is active." } };

    std::ofstream out { filename };
    serial::serialise_scene(out, m_scene);

    return std::nullopt;
}

void application::render() {

    // Get a reference to the current camera
    std::optional<camera*> res = m_scene->get_camera();

    static bool no_camera = false;
    if (res.has_value() == false) {
#ifndef NDEBUG
        if (!no_camera) std::cerr << "Warning: no camera in the scene is rendering" << std::endl;
        no_camera = true;
#endif
        return;
    } else no_camera = false;

    if (m_scene == nullptr) return;

    camera* cam = res.value();

    // Get references to the scene's lights
    std::vector<directional_light*> d_lights = m_scene->get_directional_lights();
    std::vector<point_light*> p_lights = m_scene->get_point_lights();

    // Shadow pass
    render_shadows(cam, d_lights, p_lights);

    // Lighting pass
    render_lighting(cam, d_lights, p_lights);
}

void application::render_lighting(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights) {

    // Skybox colour
    glm::vec3 night { 0.2, 0.2, 0.4 };
    glm::vec3 day { 0.4, 0.4, 0.75 };
    glm::vec3 now = day + (night - day) * (-sin(time()) * 0.5f + 0.5f);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Enable shadow texture
    m_shadowmap.bind_for_reading(SHADOW_TEX_UNIT0);

    glViewport(0, 0, width(), height());
    glClearColor(now.r, now.g, now.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    m_lightpipeline.enable();


    for (directional_light* d : d_lights) {
        if (d->shadow_caster) {
            glm::mat4 mat { d->get_shadow_matrix() };
            m_lightpipeline.set_uniform(pipeline::UNIFORM_SHADOW0_MAT, mat);
            break;
        }
    }
    
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_DIFFUSE, DIFFUSE_TEX_UNIT_INDEX);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_SPECULAR, SPECULAR_TEX_UNIT_INDEX);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_SHADOW0, SHADOW_TEX_UNIT0_INDEX);
    
    // Camera uniforms
    glm::mat4 view_mat { cam->get_view_matrix() };
    glm::mat4 proj_mat { cam->get_perspective_matrix() };
    ///// Orthographic projection:
    // glm::mat4 proj_mat { glm::ortho(-1 * 16/9.0f, 1* 16/9.0f, -1.0f, 1.0f, -10.0f, 10.0f) };

    m_lightpipeline.set_uniform(pipeline::UNIFORM_VIEW_MAT, view_mat);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_PROJ_MAT, proj_mat);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_CAMERA, cam->position());

    // Miscellaneous
    m_lightpipeline.set_uniform(pipeline::UNIFORM_TIME, time());
    
    // Set light uniforms
    m_lightpipeline.set_uniform(pipeline::UNIFORM_DIR_LIGHTS, d_lights);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_POINT_LIGHTS, p_lights);
    
    // Tell scene elements to recursively render themselves
    m_scene->render(this, &m_lightpipeline);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    gl_error_check_barrier
}

void application::render_shadows(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights) {
    
    m_shadowmap.bind_for_writing();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    m_shadowpipeline.enable();

    for (directional_light* d : d_lights) {
        if (d->shadow_caster) {
            glm::mat4 mat { d->get_shadow_matrix() };
            m_shadowpipeline.set_uniform(pipeline::UNIFORM_SHADOW0_MAT, mat);
            break;
        }
    }

    m_scene->render(this, &m_shadowpipeline);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    gl_error_check_barrier
}
