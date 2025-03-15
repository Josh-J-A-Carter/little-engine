#include <string>

#include "serialise.h"
#include "scene_node.h"
#include "utilities.h"
#include "parse_types.h"

// Dynamically generated
#include "script.h"

#include <iostream>


namespace serial {

    option<scene_node*, error> deserialise_type(arena& arena, node* n, std::string type) {
        // dummy statement so that I can more easily loop with 'else if's.
        if (type == "empty");

        // The following is dynamically generated
//
        else if (type == "script") {
            option<script*, error> res = deserialise_ref<script>(arena, n);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);

            scene_node* sc = arena.allocate<scene_node>();
            sc->component_type = scene_node_type::script;
            sc->component = std::get<script*>(res);
            return sc;
        }
//

// REGISTER_PARSE_REF(T) =>
        // else if (type == "{T}") {
        //     option<{T}*, error> res = deserialise_ref<{T}>(arena, n);
        //     if (std::holds_alternative<error>(res)) return std::get<error>(res);

        //     scene_node* sc = arena.allocate<scene_node>();
        //     sc->component_type = scene_node_type::{T};
        //     sc->component = std::get<{T}*>(res);
        //     return sc;
        // }
// 

        return error { "Type '" + type + "' not recognised when deserialising JSON to scene node." };
    }

    option<scene_node*, error> deserialise(arena& arena, node* n) {
        if (object_node* obj = dynamic_cast<object_node*>(n)) {
            option<node*, error> attr_type__res = get_node_attr(obj, "type");
            if (std::holds_alternative<error>(attr_type__res)) return std::get<error>(attr_type__res);
            if (primitive_node* p = dynamic_cast<primitive_node*>(std::get<node*>(attr_type__res))) {
                // Finally, after all that unwrapping, we have the name of the type for this node.
                // Now, we need to call the corresponding deserialisation method
                std::string type = p->entry;
                return deserialise_type(arena, n, type);
            }
            
            else return error { "'Type' attribute contains data unrelated to the node's type" };
        }
        
        return error { "Failed to parse node structure to scene; the node did not contain a JSON object." };
    }
}