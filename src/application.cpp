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
#include "texture.h"
#include "renderer.h"

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
    m_window_height = screen_width * DEFAULT_ASPECT;

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
        }, STANDARD_PIPELINE);
    
    m_shadowpipeline.initialise({
            { GL_VERTEX_SHADER, "shaders/shadow.vs" },
            { GL_FRAGMENT_SHADER, "shaders/shadow.fs" }
        }, STANDARD_PIPELINE);

    // Set up pipeline
    m_waterpipeline.initialise({
            { GL_VERTEX_SHADER, "shaders/water.vs" },
            { GL_FRAGMENT_SHADER, "shaders/water.fs" }
        }, WATER_PIPELINE);

    // Set up FBOs
    m_shadowmap.initialise(DEFAULT_SHADOW_MAP_WIDTH, DEFAULT_SHADOW_MAP_HEIGHT, true, false, true);
    m_refractionmap.initialise(DEFAULT_REFRACTION_MAP_WIDTH, DEFAULT_REFRACTION_MAP_HEIGHT, true, true, false);
    m_reflectionmap.initialise(DEFAULT_REFLECTION_MAP_WIDTH, DEFAULT_REFLECTION_MAP_HEIGHT, false, true, false);
    
    // Textures
    m_noise_texture = new texture(GL_TEXTURE_2D, "assets/noise.png");
    m_dudv_texture = new texture(GL_TEXTURE_2D, "assets/dudv.png");
    m_noise_texture->load();
    m_dudv_texture->load();
}

void application::destroy() {
    if (m_scene) save_scene();

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

std::optional<error> application::save_scene() {
    if (m_scene == nullptr) return { error { "Attempted to saved scene, but no scene is active." } };

    std::ofstream out { m_scene->filename };
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

    // View & projection matrices for camera & lights
    glm::mat4 view_mat { cam->get_view_matrix() };
    glm::mat4 proj_mat { cam->get_perspective_matrix() };
    glm::mat4 shadow_mat {};

    for (directional_light* d : d_lights) {
        if (d->shadow_caster) {
            shadow_mat = d->get_shadow_matrix(cam, view_mat);
            break;
        }
    }

    // Shadow pass
    render_shadows(cam, d_lights, p_lights, shadow_mat);

    // Lighting pass
    render_lighting(cam, d_lights, p_lights, view_mat, proj_mat, shadow_mat);

    // Water pass
    std::optional<renderer*> water = m_scene->get_water_renderer();
    if (water.has_value()) render_water(cam, d_lights, p_lights, view_mat, proj_mat, shadow_mat, water.value());
}

void application::render_lighting(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights,
                                    glm::mat4& view_mat, glm::mat4& proj_mat, glm::mat4& shadow_mat, bool external_setup = false) {

    // Skybox colour
    glm::vec3 night { 0.2, 0.2, 0.4 };
    glm::vec3 day { 0.4, 0.4, 0.75 };
    glm::vec3 now = day + (night - day) * (-sin(time()) * 0.5f + 0.5f);

    
    if (external_setup == false) {
        m_lightpipeline.enable();
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width(), height());
    }

    // Enable shadow texture
    m_shadowmap.bind_depth_for_reading(SHADOW_TEX_UNIT0);

    // Enable noise texture
    m_noise_texture->bind(NOISE_TEX_UNIT);

    glClearColor(now.r, now.g, now.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_DIFFUSE, DIFFUSE_TEX_UNIT_INDEX);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_SPECULAR, SPECULAR_TEX_UNIT_INDEX);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_SHADOW0, SHADOW_TEX_UNIT0_INDEX);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_NOISE, NOISE_TEX_UNIT_INDEX);
    
    // Camera uniforms
    m_lightpipeline.set_uniform(pipeline::UNIFORM_VIEW_MAT, view_mat);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_PROJ_MAT, proj_mat);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_SHADOW0_MAT, shadow_mat);

    m_lightpipeline.set_uniform(pipeline::UNIFORM_CAMERA, cam->position());

    // Miscellaneous
    m_lightpipeline.set_uniform(pipeline::UNIFORM_TIME, time());
    
    // Set light uniforms
    m_lightpipeline.set_uniform(pipeline::UNIFORM_DIR_LIGHTS, d_lights);
    m_lightpipeline.set_uniform(pipeline::UNIFORM_POINT_LIGHTS, p_lights);
    
    // Tell scene elements to recursively render themselves
    m_scene->render(this, &m_lightpipeline);

    if (external_setup == false) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    gl_error_check_barrier
}

void application::render_shadows(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights,
                                    glm::mat4& shadow_mat) {
    
    m_shadowmap.bind_for_writing();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);

    m_shadowpipeline.enable();

    m_shadowpipeline.set_uniform(pipeline::UNIFORM_SHADOW0_MAT, shadow_mat);

    m_scene->render(this, &m_shadowpipeline);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    gl_error_check_barrier
}

void application::render_water(camera* cam, std::vector<directional_light*>& d_lights, std::vector<point_light*>& p_lights,
                                    glm::mat4& view_mat, glm::mat4& proj_mat, glm::mat4& shadow_mat, renderer* water) {

    // Need smaller light passes before main water pass
    m_lightpipeline.enable();
    m_lightpipeline.set_uniform(pipeline::UNIFORM_CLIP_ENABLED, 1.0f);
    
    // Reflection pass
    m_reflectionmap.bind_for_writing();
    
    const float epsilon = 0.0f;
    glm::vec4 reflect_normal { 0, 1, 0, -(water->m_transform.pos.y + epsilon) };
    m_lightpipeline.set_uniform(pipeline::UNIFORM_CLIP_PLANE, reflect_normal);

    float d = 2 * (cam->m_pos.y - water->m_transform.pos.y);
    cam->m_pos.y -= d;
    float pitch = cam->m_mouse.y;
    cam->rotate({0, -2 * pitch}, false);
    glm::mat4 reflect_view { cam->get_view_matrix() };
    
    render_lighting(cam, d_lights, p_lights, reflect_view, proj_mat, shadow_mat, true);
    cam->m_pos.y += d;
    cam->rotate({0, 2 * pitch}, false);

    // Refraction pass
    m_refractionmap.bind_for_writing();

    glm::vec4 refract_normal { 0, -1, 0, water->m_transform.pos.y + epsilon };
    m_lightpipeline.set_uniform(pipeline::UNIFORM_CLIP_PLANE, refract_normal);

    render_lighting(cam, d_lights, p_lights, view_mat, proj_mat, shadow_mat, true);

    // Clean up
    m_lightpipeline.set_uniform(pipeline::UNIFORM_CLIP_ENABLED, 0.0f);


    // Render the water to the main FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width(), height());

    glEnable(GL_DEPTH_TEST);

    m_reflectionmap.bind_color_for_reading(REFLECT_TEX_UNIT);
    m_refractionmap.bind_color_for_reading(REFRACT_TEX_UNIT);
    m_dudv_texture->bind(DUDV_TEX_UNIT);

    m_waterpipeline.enable();

    m_waterpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_REFLECTION, REFLECT_TEX_UNIT_INDEX);
    m_waterpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_REFRACTION, REFRACT_TEX_UNIT_INDEX);
    m_waterpipeline.set_uniform(pipeline::UNIFORM_SAMPLER_DUDV, DUDV_TEX_UNIT_INDEX);

    m_waterpipeline.set_uniform(pipeline::UNIFORM_VIEW_MAT, view_mat);
    m_waterpipeline.set_uniform(pipeline::UNIFORM_PROJ_MAT, proj_mat);

    m_waterpipeline.set_uniform(pipeline::UNIFORM_CAMERA, cam->position());

    m_waterpipeline.set_uniform(pipeline::UNIFORM_TIME, time());

    m_scene->render(this, &m_waterpipeline);

    gl_error_check_barrier
}
