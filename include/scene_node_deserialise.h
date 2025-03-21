#ifndef SCENE_NODE_DESERIALISE_H
#define SCENE_NODE_DESERIALISE_H

#include "scene_node.h"
#include "serialise.h"

namespace serial {
    inline scene_node* find_ref_recursive(scene_node* root, int id) {
        if (root->id == id) return root;

        for (scene_node* child : root->children) {
            if (find_ref_recursive(child, id)) return child;
        }

        return nullptr;
    }

    template <>
    inline option<scene_node*, error> deserialise_ref<scene_node>(arena& arena, scene_node* root, node* n) {
        if (primitive_node* p = dynamic_cast<primitive_node*>(n)) {
            int id = std::stoi(p->entry);
            scene_node* res = find_ref_recursive(root, id);
            if (res == nullptr) return { error { "Scene node reference field could not be instantiated; no node with ID " + id } };
            return res;
        }

        return { error { "Scene node reference field does not contain an ID." } };
    }
}

#endif