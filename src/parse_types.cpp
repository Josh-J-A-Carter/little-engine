#include <string>
#include <optional>
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
    std::optional<error> deserialise_type(arena& arena, scene_node* sc, scene_node* root, node* n, std::string type) {
        if (type == "empty") {
            sc->component_type = scene_node_type::empty;
            return std::nullopt;
        }

        // Dynamic else-ifs
        else if (type == "renderer") {
            option<renderer*, error> res = deserialise_ref<renderer>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            renderer* obj = std::get<renderer*>(res);
            sc->component_type = scene_node_type::renderer;
            sc->component = obj;
            sc->cmp_load = [](scene_node* sc) { load(static_cast<renderer*>(sc->component)); };
            sc->cmp_run = [](scene_node* sc) { run(static_cast<renderer*>(sc->component)); };
            sc->cmp_render = [](scene_node* sc, pipeline& p) { render(static_cast<renderer*>(sc->component), p); };
            return std::nullopt;
        }

        else if (type == "script") {
            option<script*, error> res = deserialise_ref<script>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            script* obj = std::get<script*>(res);
            sc->component_type = scene_node_type::script;
            sc->component = obj;
            sc->cmp_load = [](scene_node* sc) { load(static_cast<script*>(sc->component)); };
            sc->cmp_run = [](scene_node* sc) { run(static_cast<script*>(sc->component)); };
            sc->cmp_render = [](scene_node* sc, pipeline& p) { render(static_cast<script*>(sc->component), p); };
            return std::nullopt;
        }

        else if (type == "transform") {
            option<transform*, error> res = deserialise_ref<transform>(arena, root, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            transform* obj = std::get<transform*>(res);
            sc->component_type = scene_node_type::transform;
            sc->component = obj;
            sc->cmp_load = [](scene_node* sc) { load(static_cast<transform*>(sc->component)); };
            sc->cmp_run = [](scene_node* sc) { run(static_cast<transform*>(sc->component)); };
            sc->cmp_render = [](scene_node* sc, pipeline& p) { render(static_cast<transform*>(sc->component), p); };
            return std::nullopt;
        }


        return error { "Type '" + type + "' not recognised when deserialising JSON to scene node." };
    }

    void serialise_node(std::ostream& os, const scene_node* sc, int indt) {
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

