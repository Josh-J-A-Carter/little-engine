#include <string>
#include "serialise.h"
#include "scene_node.h"
#include "utilities.h"
#include "parse_types.h"

// Dynamically generated includes
#include "script.h"

namespace serial {
    option<scene_node*, error> deserialise_type(arena& arena, node* n, std::string type) {
        if (type == "empty") {
            scene_node* sc = arena.allocate<scene_node>();
            sc->component_type = scene_node_type::empty;
            return sc;
        }

        // Dynamic else-ifs
        else if (type == "script") {
            option<script*, error> res = deserialise_ref<script>(arena, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            scene_node* sc = arena.allocate<scene_node>();
            script* obj = std::get<script*>(res);
            sc->component_type = scene_node_type::script;
            sc->component = obj;
            sc->cmp_load = [](scene_node* sc) { load(static_cast<script*>(sc->component)); };
            sc->cmp_run = [](scene_node* sc) { run(static_cast<script*>(sc->component)); };
            sc->cmp_render = [](scene_node* sc) { render(static_cast<script*>(sc->component)); };
            return sc;
        }


        return error { "Type '" + type + "' not recognised when deserialising JSON to scene node." };
    }

    void serialise(std::ostream& os, const scene_node* sc, const scene_node* _, int indt) {
        if (sc->component_type == scene_node_type::empty) os << "{ type: empty }";

        // Dynamic serialisation stuff; scene nodes don't know how to serialise their component
        // as they don't know its type. Why didn't I just use polymorphism to be honest??? Too late!
        else if (sc->component_type == scene_node_type::script) {
            serialise(os, *static_cast<script*>(sc->component), sc, indt);
        }

    }
}

