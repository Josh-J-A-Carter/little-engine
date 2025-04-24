#ifndef CAMERA_H
#define CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "serialise.h"
#include "application.h"

struct scene;

struct camera {
    glm::vec3 m_pos { 0, 0, 0 };
    glm::vec3 m_forward { 0, 0, -1 };
    glm::vec3 m_up { 0, 1, 0 };

    glm::vec3 m_facing { m_forward };

    glm::vec2 m_mouse { 0, 0 };
    float m_sensitivity { 0.1 };
    float m_speed { 1 };

    float m_shadow_range { 100 };
    
    float m_near { 0.1f };
    float m_far { 100 };
    float m_fov { 65 };
    float m_aspect { 1 };

    camera() {}

    // Builder members, for cascading instantiation
    camera& init_pos(glm::vec3 pos) { m_pos = pos ; return *this; }
    camera& init_forward(glm::vec3 forward) { m_forward = forward ; return *this; }
    camera& init_up(glm::vec3 up) { m_up = up ; return *this; }
    camera& init_fov(float fov) { m_fov = fov ; return *this; }
    camera& init_aspect(float aspect) { m_aspect = aspect ; return *this; }
    camera& init_clip(float near, float far) { m_near = near ; m_far = far ; return *this; }

    void translate(glm::vec3 delta);

    glm::vec3 position();

    void rotate(glm::vec2 delta, bool constraint_check = true);

    glm::vec2 rotation();

    glm::mat4 get_view_matrix() const;

    glm::mat4 get_perspective_matrix() const;

    glm::vec3 up() const;

    glm::vec3 forward() const;

    glm::vec3 left() const;
};

template<>
inline void load<camera>(application* app, scene* scene, scene_node* this_node, camera* cam) {
    cam->init_aspect({ app->aspect() });
    cam->init_clip(cam->m_near, cam->m_far);
}


template<>
inline void run<camera>(application* app, scene* scene, scene_node* this_node, camera* cam) {
    cam->init_aspect({ app->aspect() });

    float dt = app->delta_time();

    static bool escaped = false;
    static bool just_pressed_escape = false;

    if (!escaped && !app->unfocused) {
        SDL_WarpMouseInWindow(app->window(), app->width() / 2, app->height() / 2);
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            app->quit();
            return;
        }

        if (escaped || app->unfocused) continue;

        if (event.type == SDL_MOUSEMOTION) {
            cam->rotate({ -event.motion.xrel * dt / app->desired_fps, -event.motion.yrel * dt / app->desired_fps });
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (!escaped && !app->unfocused) {
        float speed { cam->m_speed * -0.1f * dt / app->desired_fps };

        if (state[SDL_SCANCODE_LCTRL]) speed *= 3;

        if (state[SDL_SCANCODE_W]) cam->translate(cam->forward() * speed);
        else if (state[SDL_SCANCODE_S]) cam->translate(cam->forward() * -speed);

        if (state[SDL_SCANCODE_A]) cam->translate(cam->left() * speed);
        else if (state[SDL_SCANCODE_D]) cam->translate(cam->left() * -speed);

        if (state[SDL_SCANCODE_SPACE]) cam->translate(cam->up() * speed);
        else if (state[SDL_SCANCODE_LSHIFT]) cam->translate(cam->up() * -speed);
    }

#ifndef __EMSCRIPTEN__
    if (state[SDL_SCANCODE_ESCAPE] && !just_pressed_escape) {
        escaped = !escaped;
        just_pressed_escape = true;
    }

    if (just_pressed_escape && !state[SDL_SCANCODE_ESCAPE]) {
        just_pressed_escape = false;
    }
#else
    if (state[SDL_SCANCODE_ESCAPE]) {
        app->unfocused = true;
    }
#endif
}

REGISTER_PARSE_REF(camera)

namespace serial {
    inline void serialise(std::ostream& os, const camera& obj, const scene_node* sc, int indt) {
        serial::serialiser<camera> sr = { os, obj, sc, indt };

        REPORT(sr, m_pos)
        REPORT(sr, m_forward)
        REPORT(sr, m_up)
        REPORT(sr, m_facing)

        REPORT(sr, m_near)
        REPORT(sr, m_far)
        REPORT(sr, m_fov)
        // REPORT(sr, m_aspect)
        REPORT(sr, m_sensitivity)
        REPORT(sr, m_speed)
        REPORT(sr, m_mouse)

        REPORT(sr, m_shadow_range)
    }

    template <>
    inline option<camera*, error> deserialise_ref<camera>(arena& arena, scene_node* root, node* n) {
        camera* r = arena.allocate<camera>();

        DESERIALISE_VAL(r, n, m_pos)
        DESERIALISE_VAL(r, n, m_forward)
        DESERIALISE_VAL(r, n, m_up)
        DESERIALISE_VAL(r, n, m_facing)

        DESERIALISE_VAL(r, n, m_near)
        DESERIALISE_VAL(r, n, m_far)
        DESERIALISE_VAL(r, n, m_fov)
        // DESERIALISE_VAL(r, n, m_aspect)
        DESERIALISE_VAL(r, n, m_sensitivity)
        DESERIALISE_VAL(r, n, m_speed)
        DESERIALISE_VAL(r, n, m_mouse)

        DESERIALISE_VAL(r, n, m_shadow_range)

        return r;
    }
}


#endif