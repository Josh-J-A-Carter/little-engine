#ifndef SERIALISE_H
#define SERIALISE_H

#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>
#include <string>
#include <optional>

#include <glm/vec3.hpp>

#include "utilities.h"
#include "arena.h"
#include "scene.h"
#include "scene_node.h"

#include "parse_declarations.h"

namespace serial {

    // 
    // 
    // Deserialisation
    // 
    // 

    class node {
        public:
            virtual void print() {}
    };

    class primitive_node : public node {
        public:
            std::string entry {};

            void print() {
                std::cout << entry << std::endl;
            }
    };

    class array_node : public node {
        public:
            std::vector<node*> entries;

            void print() {
                std::cout << "[" << std::endl;
                for (node* n : entries) n->print();
                std::cout << "]" << std::endl;
            }
    };

    class object_node : public node {
        public:
            std::vector<std::pair<std::string, node*>> attributes;

            void print() {
                std::cout << "{" << std::endl;
                for (std::pair<std::string_view, node*> attr : attributes) {
                    std::cout << attr.first << ":::" << std::endl;
                    attr.second->print();
                }
                std::cout << "}" << std::endl;
            }
    };


    option<scene*, error> read_scene_from_file(std::string filename);

    template<typename T>
    option<T, error> deserialise_val(arena& arena, node* n);

    template<>
    inline option<int, error> deserialise_val<int>(arena& arena, node* n) {
        primitive_node* p { static_cast<primitive_node*>(n) };
        return std::stoi(p->entry);
    }

    template<>
    inline option<float, error> deserialise_val<float>(arena& arena, node* n) {
        primitive_node* p { static_cast<primitive_node*>(n) };
        return std::stof(p->entry);
    }

    template<>
    inline option<std::string, error> deserialise_val<std::string>(arena& arena, node* n) {
        primitive_node* p { static_cast<primitive_node*>(n) };
        return p->entry;
    }

    template<>
    inline option<glm::vec3, error> deserialise_val<glm::vec3>(arena& arena, node* n) {
        array_node* a { static_cast<array_node*>(n) };
        glm::vec3 out {};

        if (a->entries.size() != 3) return { error { "glm::vec3 does not contain three entries." } };

        for (int index = 0 ; index < 3 ; index += 1) {
            node* entry = a->entries[index];
            option<float, error> res = deserialise_val<float>(arena, entry);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            out[index] = std::get<float>(res);
        }

        return out;
    }


    template<typename T>
    option<T*, error> deserialise_ref(arena& arena, scene_node* root, node* n);

    inline option<node*, error> get_node_attr(object_node* n, std::string attr_name) {
        for (int i = 0 ; i < n->attributes.size() ; i += 1) {
            if (n->attributes.at(i).first == attr_name) return n->attributes.at(i).second;
        }

        return { error { "Unable to find field " + attr_name } };
    }

    template<typename T>
    option<std::vector<T>, error> deserialise_vec_val(arena& arena, node* n) {
        array_node* a { static_cast<array_node*>(n) };
        std::vector<T> arr {};

        for (node* entry : a->entries) {
            option<T, error> res = deserialise_val<T>(arena, entry);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            arr.push_back(std::get<T>(res));
        }

        return arr;
    }

    template<typename T>
    option<std::vector<T*>, error> deserialise_vec_ref(arena& arena, scene_node* root, node* n) {
        array_node* a { static_cast<array_node*>(n) };
        std::vector<T> arr {};

        for (node* entry : a->entries) {
            option<T*, error> res = deserialise_ref<T>(arena, root, entry);
            if (std::holds_alternative<error>(res)) return std::get<error>(res);
            arr.push_back(std::get<T*>(res));
        }

        return arr;
    }

    std::optional<error> deserialise_type(arena& arena, scene_node* sc, scene_node* root, node* n, std::string type);
}

// Utility macro for deserialising a field from a node hierarchy that has been extracted from a file.
// First, it finds the node inside the hierarchy that corresponds to the desired field.
// Then, it parses that node to the required type, and stores the result in the field.
#define DESERIALISE_REF(obj, arena, root, n, field) \
    option<node*, error> res__attr_##field = get_node_attr(static_cast<object_node*>(n), #field); \
    if (std::holds_alternative<error>(res__attr_##field)) return std::get<error>(res__attr_##field); \
    node* n__##field = std::get<node*>(res__attr_##field); \
    using t__##field = std::remove_reference_t<decltype(*(obj->field))>; \
    option<t__##field*, error> res__ds_##field = deserialise_ref<t__##field>(arena, root, n__##field); \
    if (std::holds_alternative<error>(res__ds_##field)) return std::get<error>(res__ds_##field); \
    obj->field = std::get<t__##field*>(res__ds_##field);

#define DESERIALISE_VEC_REF(obj, arena, root, n, field) \
    option<node*, error> res__##field = get_node_attr(static_cast<object_node*>(n), #field); \
    if (std::holds_alternative<error>(res__##field)) return std::get<error>(res__##field); \
    node* n__##field = std::get<node*>(res__##field); \
    using t__##field = decltype(obj->field)::value_type; \
    option<std::vector<t__##field>, error> res__ds_##field = deserialise_vec_ref<t__##field>(arena, root, n__##field); \
    if (std::holds_alternative<error>(res__ds_##field)) return std::get<error>(res__ds_##field); \
    obj->field = std::get<std::vector<t__##field>>(res__ds_##field);

#define DESERIALISE_VEC_VAL(obj, n, field) \
    option<node*, error> res__##field = get_node_attr(static_cast<object_node*>(n), #field); \
    if (std::holds_alternative<error>(res__##field)) return std::get<error>(res__##field); \
    node* n__##field = std::get<node*>(res__##field); \
    using t__##field = decltype(obj->field)::value_type; \
    option<std::vector<t__##field>, error> res__ds_##field = deserialise_vec_val<t__##field>(arena, n__##field); \
    if (std::holds_alternative<error>(res__ds_##field)) return std::get<error>(res__ds_##field); \
    obj->field = std::get<std::vector<t__##field>>(res__ds_##field);

#define DESERIALISE_VAL(obj, n, field) \
    option<node*, error> res__##field = get_node_attr(static_cast<object_node*>(n), #field); \
    if (std::holds_alternative<error>(res__##field)) return std::get<error>(res__##field); \
    node* n__##field = std::get<node*>(res__##field); \
    using t__##field = decltype(obj->field); \
    option<t__##field, error> res__ds_##field = deserialise_val<t__##field>(arena, n__##field); \
    if (std::holds_alternative<error>(res__ds_##field)) return std::get<error>(res__ds_##field); \
    obj->field = std::get<t__##field>(res__ds_##field);

#include "scene_node_deserialise.h"

// 
// 
// Serialisation
// 
// 

#define REGISTER_PARSE_TYPE(X) template <> struct serial::TypeParseTraits<X> \
    { static const char* name; } ; inline const char* serial::TypeParseTraits<X>::name = #X;

#define REGISTER_PARSE_REF(X) template <> struct serial::TypeParseTraits<X> \
    { static const char* name; } ; inline const char* serial::TypeParseTraits<X>::name = #X;


#define REPORT(SERIALISER, FIELD) SERIALISER.report(#FIELD, SERIALISER.object.FIELD);

namespace serial {

    inline std::string indent(int n) {
        return std::string(2 * n, ' ');
    }

    template<typename T>
    struct TypeParseTraits;

    void serialise_scene(std::ostream& os, const scene* sc);

    void serialise_node(std::ostream& os, const scene_node* sc, int indt);

    void serialise_node_list(std::ostream& os, const std::vector<scene_node*> list, int indt);

    void serialise_node_empty(std::ostream& os, const scene_node* sc, int indt);

    void serialise(std::ostream& os, const scene_node* sc, const scene_node* _, int indt);

    inline void serialise(std::ostream& os, float s, const scene_node* _, int indt) {
        os << s;
    }

    inline void serialise(std::ostream& os, int s, const scene_node* _, int indt) {
        os << s;
    }

    inline void serialise(std::ostream& os, std::string_view s, const scene_node* _, int indt) {
        os << s;
    }

    inline void serialise(std::ostream& os, glm::vec3 v, const scene_node* _, int indt) {
        os << "[" << v[0] << ", " << v[1] << ", " << v[2] << "]";
    }

    template <typename T>
    void serialise(std::ostream& os, const std::vector<T> arr, const scene_node* sc, int indt) {
        if (arr.empty()) {
            os << "[]\n";
            return;
        }

        os << "[\n";

        int i_inner = indt + 1;
        for (int i = 0 ; i < arr.size() ; i += 1) {
            os << indent(i_inner);
            serialise(os, arr[i], sc, i_inner + 1);

            os << (i < arr.size() - 1 ? ",\n" : "\n");
        }

        os << indent(indt) << "]";
    }

    template <typename S>
    struct serialiser {
        private:
            std::ostream& os;
            int indt { 0 };
            const scene_node* sc { nullptr };
                    
        public:
            const S& object;

            serialiser(std::ostream& os, const S& object, int indt) : os { os }, object { object }, indt { indt } {
                os << "{\n";
                os << indent(indt + 1) << "type: " << TypeParseTraits<S>::name;
            }

            serialiser(std::ostream& os, const S& object, const scene_node* sc, int indt)
                    : os { os }, object { object }, sc { sc }, indt { indt } {
                os << "{\n";
                os << indent(indt + 1) << "type: " << TypeParseTraits<S>::name;
                if (sc) os << ",\n" << indent(indt + 1) << "id: " << sc->id;
                if (sc) os << ",\n" << indent(indt + 1) << "name: " << sc->name;
            }

            ~serialiser() {
                // If this is coming from a scene-node, include child scene_nodes as an attribute
                if (sc) {
                    os << ",\n" << indent(indt + 1) << "children: ";
                    serialise_node_list(os, sc->children, indt + 2);
                }

                os << "\n" << indent(indt) << "}";
            }

            template <typename T>
            inline void report(std::string field_name, T field_value) {
                os << ",\n";
                os << indent(indt + 1) << field_name << ": ";
                // Always send nullptr for the scene_node*, since we don't want child objects to reuse it
                // - if the child objects are themselves scene_node*, then they can just use themselves
                serialise(os, field_value, nullptr, indt + 2);
            }
    };
}

#endif