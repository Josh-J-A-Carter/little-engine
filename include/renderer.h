#ifndef RENDERER_H
#define RENDERER_H

#include <iostream>

#include "arena.h"
#include "scene_node.h"
#include "transform.h"
#include "mesh.h"
#include "serialise.h"
#include "parse_declarations.h"
#include "pipeline.h"

struct renderer {
    transform m_transform {};
    mesh m_mesh {};
    std::string filename {};

    int m_pipeline { STANDARD_PIPELINE };
};

struct application;
struct scene;

template<>
inline void load<renderer>(application* app, scene* scene, scene_node* this_node, renderer* r) {
    r->m_mesh.load(r->filename);
}

template<>
inline void render<renderer>(application* app, scene* scene, scene_node* this_node, renderer* r, pipeline* p) {

    // Only render if the renderer uses this pipeline
    if (r->m_pipeline != p->identifier()) return;

    // Model matrix
    glm::mat4 model_mat { r->m_transform.get_model_matrix() };
    p->set_uniform(pipeline::UNIFORM_MODEL_MAT, model_mat);

    // Ambient material
    p->set_uniform(pipeline::UNIFORM_MATERIAL, r->m_mesh.get_material());

    // Draw call
    r->m_mesh.render();
}

REGISTER_PARSE_REF(renderer)

namespace serial {
    inline void serialise(std::ostream& os, const renderer& obj, const scene_node* sc, int indt) {
        serial::serialiser<renderer> sr = { os, obj, sc, indt };

        REPORT(sr, m_transform)
        REPORT(sr, filename)
        REPORT(sr, m_pipeline)
    }

    template <>
    inline option<renderer*, error> deserialise_ref<renderer>(arena& arena, scene_node* root, node* n) {
        renderer* r = arena.allocate<renderer>();

        DESERIALISE_VAL(r, n, m_transform)
        DESERIALISE_VAL(r, n, filename)
        DESERIALISE_VAL(r, n, m_pipeline)

        return r;
    }
}

#endif