#include <string>
#include "serialise.h"
#include "scene_node.h"
#include "utilities.h"
#include "parse_types.h"

// Dynamically generated includes
#include "renderer.h"
#include "script.h"
#include "transform.h"

struct pipeline;

namespace serial {
    option<scene_node*, error> deserialise_type(arena& arena, node* n, std::string type) {
        if (type == "empty") {
            scene_node* sc = arena.allocate<scene_node>();
            sc->component_type = scene_node_type::empty;
            return sc;
        }

        // Dynamic else-ifs
        else if (type == "renderer") {
            option<renderer*, error> res = deserialise_ref<renderer>(arena, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            scene_node* sc = arena.allocate<scene_node>();
            renderer* obj = std::get<renderer*>(res);
            sc->component_type = scene_node_type::renderer;
            sc->component = obj;
            sc->cmp_load = [](scene_node* sc) { load(static_cast<renderer*>(sc->component)); };
            sc->cmp_run = [](scene_node* sc) { run(static_cast<renderer*>(sc->component)); };
            sc->cmp_render = [](scene_node* sc, pipeline& p) { render(static_cast<renderer*>(sc->component), p); };
            return sc;
        }

        else if (type == "script") {
            option<script*, error> res = deserialise_ref<script>(arena, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            scene_node* sc = arena.allocate<scene_node>();
            script* obj = std::get<script*>(res);
            sc->component_type = scene_node_type::script;
            sc->component = obj;
            sc->cmp_load = [](scene_node* sc) { load(static_cast<script*>(sc->component)); };
            sc->cmp_run = [](scene_node* sc) { run(static_cast<script*>(sc->component)); };
            sc->cmp_render = [](scene_node* sc, pipeline& p) { render(static_cast<script*>(sc->component), p); };
            return sc;
        }

        else if (type == "transform") {
            option<transform*, error> res = deserialise_ref<transform>(arena, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            scene_node* sc = arena.allocate<scene_node>();
            transform* obj = std::get<transform*>(res);
            sc->component_type = scene_node_type::transform;
            sc->component = obj;
            sc->cmp_load = [](scene_node* sc) { load(static_cast<transform*>(sc->component)); };
            sc->cmp_run = [](scene_node* sc) { run(static_cast<transform*>(sc->component)); };
            sc->cmp_render = [](scene_node* sc, pipeline& p) { render(static_cast<transform*>(sc->component), p); };
            return sc;
        }


        return error { "Type '" + type + "' not recognised when deserialising JSON to scene node." };
    }

    void serialise(std::ostream& os, const scene_node* sc, const scene_node* _, int indt) {
        if (sc->component_type == scene_node_type::empty) os << "{ type: empty }";

        // Dynamic serialisation stuff; scene nodes don't know how to serialise their component
        // as they don't know its type. Why didn't I just use polymorphism to be honest??? Too late!
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

