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
           sc->component_type = scene_node_type::script;
           sc->component = std::get<script*>(res);
           return sc;
       }


       return error { "Type '" + type + "' not recognised when deserialising JSON to scene node." };
   }
}

