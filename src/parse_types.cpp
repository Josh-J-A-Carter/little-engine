#include <string>
#include <optional>
#include "serialise.h"
#include "scene_node.h"
#include "utilities.h"
#include "parse_types.h"

// Dynamically generated includes
#include "camera.h"
#include "directional_light.h"
#include "light.h"
#include "point_light.h"
#include "renderer.h"
#include "script.h"
#include "transform.h"

struct pipeline;
struct application;

namespace serial {
    std::optional<error> deserialise_type(arena& arena, scene_node* sc, scene_node* root, node* n, std::string type) {
        if (type == "empty") {
            sc->component_type = scene_node_type::empty;
            return std::nullopt;
        }

        // Dynamic else-ifs
        else if (type == "camera") {
            option<camera*, error> res = deserialise_ref<camera>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            camera* obj = std::get<camera*>(res);
            sc->component_type = scene_node_type::camera;
            sc->component = obj;
            sc->cmp_load = [](application* app, scene* scene, scene_node* this_node) {
                    load(app, scene, this_node, static_cast<camera*>(this_node->component)); };
            sc->cmp_run = [](application* app, scene* scene, scene_node* this_node) {
                    run(app, scene, this_node, static_cast<camera*>(this_node->component)); };
            sc->cmp_render = [](application* app, scene* scene, scene_node* this_node, pipeline* p) {
                    render(app, scene, this_node, static_cast<camera*>(this_node->component), p); };
            return std::nullopt;
        }

        else if (type == "directional_light") {
            option<directional_light*, error> res = deserialise_ref<directional_light>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            directional_light* obj = std::get<directional_light*>(res);
            sc->component_type = scene_node_type::directional_light;
            sc->component = obj;
            sc->cmp_load = [](application* app, scene* scene, scene_node* this_node) {
                    load(app, scene, this_node, static_cast<directional_light*>(this_node->component)); };
            sc->cmp_run = [](application* app, scene* scene, scene_node* this_node) {
                    run(app, scene, this_node, static_cast<directional_light*>(this_node->component)); };
            sc->cmp_render = [](application* app, scene* scene, scene_node* this_node, pipeline* p) {
                    render(app, scene, this_node, static_cast<directional_light*>(this_node->component), p); };
            return std::nullopt;
        }

        else if (type == "light") {
            option<light*, error> res = deserialise_ref<light>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            light* obj = std::get<light*>(res);
            sc->component_type = scene_node_type::light;
            sc->component = obj;
            sc->cmp_load = [](application* app, scene* scene, scene_node* this_node) {
                    load(app, scene, this_node, static_cast<light*>(this_node->component)); };
            sc->cmp_run = [](application* app, scene* scene, scene_node* this_node) {
                    run(app, scene, this_node, static_cast<light*>(this_node->component)); };
            sc->cmp_render = [](application* app, scene* scene, scene_node* this_node, pipeline* p) {
                    render(app, scene, this_node, static_cast<light*>(this_node->component), p); };
            return std::nullopt;
        }

        else if (type == "point_light") {
            option<point_light*, error> res = deserialise_ref<point_light>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            point_light* obj = std::get<point_light*>(res);
            sc->component_type = scene_node_type::point_light;
            sc->component = obj;
            sc->cmp_load = [](application* app, scene* scene, scene_node* this_node) {
                    load(app, scene, this_node, static_cast<point_light*>(this_node->component)); };
            sc->cmp_run = [](application* app, scene* scene, scene_node* this_node) {
                    run(app, scene, this_node, static_cast<point_light*>(this_node->component)); };
            sc->cmp_render = [](application* app, scene* scene, scene_node* this_node, pipeline* p) {
                    render(app, scene, this_node, static_cast<point_light*>(this_node->component), p); };
            return std::nullopt;
        }

        else if (type == "renderer") {
            option<renderer*, error> res = deserialise_ref<renderer>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            renderer* obj = std::get<renderer*>(res);
            sc->component_type = scene_node_type::renderer;
            sc->component = obj;
            sc->cmp_load = [](application* app, scene* scene, scene_node* this_node) {
                    load(app, scene, this_node, static_cast<renderer*>(this_node->component)); };
            sc->cmp_run = [](application* app, scene* scene, scene_node* this_node) {
                    run(app, scene, this_node, static_cast<renderer*>(this_node->component)); };
            sc->cmp_render = [](application* app, scene* scene, scene_node* this_node, pipeline* p) {
                    render(app, scene, this_node, static_cast<renderer*>(this_node->component), p); };
            return std::nullopt;
        }

        else if (type == "script") {
            option<script*, error> res = deserialise_ref<script>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            script* obj = std::get<script*>(res);
            sc->component_type = scene_node_type::script;
            sc->component = obj;
            sc->cmp_load = [](application* app, scene* scene, scene_node* this_node) {
                    load(app, scene, this_node, static_cast<script*>(this_node->component)); };
            sc->cmp_run = [](application* app, scene* scene, scene_node* this_node) {
                    run(app, scene, this_node, static_cast<script*>(this_node->component)); };
            sc->cmp_render = [](application* app, scene* scene, scene_node* this_node, pipeline* p) {
                    render(app, scene, this_node, static_cast<script*>(this_node->component), p); };
            return std::nullopt;
        }

        else if (type == "transform") {
            option<transform*, error> res = deserialise_ref<transform>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            transform* obj = std::get<transform*>(res);
            sc->component_type = scene_node_type::transform;
            sc->component = obj;
            sc->cmp_load = [](application* app, scene* scene, scene_node* this_node) {
                    load(app, scene, this_node, static_cast<transform*>(this_node->component)); };
            sc->cmp_run = [](application* app, scene* scene, scene_node* this_node) {
                    run(app, scene, this_node, static_cast<transform*>(this_node->component)); };
            sc->cmp_render = [](application* app, scene* scene, scene_node* this_node, pipeline* p) {
                    render(app, scene, this_node, static_cast<transform*>(this_node->component), p); };
            return std::nullopt;
        }


        return error { "Type '" + type + "' not recognised when deserialising JSON to scene node." };
    }

    void serialise_node(std::ostream& os, const scene_node* sc, int indt) {
        if (sc->component_type == scene_node_type::empty) serialise_node_empty(os, sc, indt);

        // Dynamic serialisation stuff; scene nodes don't know how to serialise their component
        // as they don't know its type. Why didn't I just use polymorphism to be honest??? Too late!
        else if (sc->component_type == scene_node_type::camera) {
            serialise(os, *static_cast<camera*>(sc->component), sc, indt);
        }

        else if (sc->component_type == scene_node_type::directional_light) {
            serialise(os, *static_cast<directional_light*>(sc->component), sc, indt);
        }

        else if (sc->component_type == scene_node_type::light) {
            serialise(os, *static_cast<light*>(sc->component), sc, indt);
        }

        else if (sc->component_type == scene_node_type::point_light) {
            serialise(os, *static_cast<point_light*>(sc->component), sc, indt);
        }

        else if (sc->component_type == scene_node_type::renderer) {
            serialise(os, *static_cast<renderer*>(sc->component), sc, indt);
        }

        else if (sc->component_type == scene_node_type::script) {
            serialise(os, *static_cast<script*>(sc->component), sc, indt);
        }

        else if (sc->component_type == scene_node_type::transform) {
            serialise(os, *static_cast<transform*>(sc->component), sc, indt);
        }

    }
}

