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


// #define NDEBUG


application g_app {};
camera g_camera {};
std::vector<mesh> g_meshes {};


void load_meshes(const texture& texture) {

    glm::vec2 t00 { 0, 0 };
    glm::vec2 t01 { 0, 1 };
    glm::vec2 t10 { 1, 0 };
    glm::vec2 t11 { 1, 1 };

    std::vector<vertex> vertices {
        { glm::vec3{ -0.5f, -0.5f, 0.0f },     t00 },
        { glm::vec3{ 0.5f, -0.5f, 0.0f },      t10 },
        { glm::vec3{ 0.5f, 0.5f, 0.0f },       t11 },
        { glm::vec3{ -0.5f, 0.5f, 0.0f },      t01 }
    };

    std::vector<GLuint> indices {
        // t1
        0, 1, 3,
        // t2
        1, 2, 3
    };

    mesh mesh1 { vertices, indices, texture };
    mesh1.m_translation = { -2, 0, -2 };

    mesh mesh2 { vertices, indices, texture };
    mesh2.m_translation = { 2, 0, -2 };

    g_meshes.push_back(mesh1);
    g_meshes.push_back(mesh2);

    std::vector<vertex> cube_verts {
        // front face
        { glm::vec3{ -0.5f, -0.5f, 0.0f },       t00 },
        { glm::vec3{ 0.5f, -0.5f, 0.0f },        t10 },
        { glm::vec3{ 0.5f, 0.5f, 0.0f },         t11 },
        { glm::vec3{ -0.5f, 0.5f, 0.0f },        t01 },

        // back face
        { glm::vec3{ -0.5f, -0.5f, -1.0f },      t10 },
        { glm::vec3{ 0.5f, -0.5f, -1.0f  },      t00 },
        { glm::vec3{ 0.5f, 0.5f, -1.0f  },       t01 },
        { glm::vec3{ -0.5f, 0.5f, -1.0f  },      t11 },

        // don't need any more vertices - just need to specify triangles in index buffer
    };

    std::vector<GLuint> cube_indices {
        // front
        0, 1, 3,
        1, 2, 3,
        // back
        4, 5, 7,
        5, 6, 7,
        // top
        3, 2, 7,
        2, 6, 7,
        // bottom
        0, 1, 4,
        1, 5, 4,
        // right
        1, 5, 6,
        1, 6, 2,
        // left
        0, 4, 7,
        0, 7, 3
    };

    mesh cube { cube_verts, cube_indices, texture };
    cube.m_translation = { 0, 0, -3 };

    g_meshes.push_back(cube);
}

bool input() {
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
        float speed { 0.001f };

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


    GLuint program = g_app.pipeline().program();
    glUseProgram(program);

    //// ------ Camera Matrices ------

    // View matrix
    glm::mat4 view_mat { g_camera.get_view_matrix() };
    GLint view_mat_location = glGetUniformLocation(program, "u_view_matrix");
    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view_mat[0][0]);

    // Perspective matrix
    glm::mat4 pers_mat { g_camera.get_perspective_matrix() };
    GLint pers_mat_location = glGetUniformLocation(program, "u_pers_matrix");
    glUniformMatrix4fv(pers_mat_location, 1, GL_FALSE, &pers_mat[0][0]);

    GLint model_mat_location = glGetUniformLocation(program, "u_model_matrix");

    GLint sampler_location = glGetUniformLocation(program, "u_sampler");

    //// -------- Mesh Data ---------

    for (mesh& mesh : g_meshes) {
        mesh.m_y_rotation += 0.01f;

        // Model matrix
        glm::mat4 model_mat { mesh.model_matrix() };
        glUniformMatrix4fv(model_mat_location, 1, GL_FALSE, &model_mat[0][0]);

        mesh.m_texture.bind(GL_TEXTURE0);
        glUniform1i(sampler_location, 0);

        glBindVertexArray(mesh.m_vertex_array_object);
        glDrawElements(GL_TRIANGLES, mesh.vert_count(), GL_UNSIGNED_INT, (GLvoid*) 0);
    }

    // Cleanup - only actually necessary if we have multiple pipelines
    glUseProgram(0);
}

void loop() {
    bool quit = false;

    while (!quit) {
        quit = input();

        draw();

        SDL_GL_SwapWindow(g_app.window());
    }
}

int main(int argv, char** args)  {
    g_app.create();

    texture brick_texture { GL_TEXTURE_2D, "assets/brick.png" };
    brick_texture.load();

    load_meshes(brick_texture);

    // Set up the camera
    g_camera = camera().init_pos({ 0, 0, 0 })
                       .init_aspect({ g_app.aspect() })
                       .init_clip(0.1, 20);

    loop();

    g_app.destroy();

    return EXIT_SUCCESS;
}