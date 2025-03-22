#ifndef PARSE_DECLARATIONS_H
#define PARSE_DECLARATIONS_H

#include <ostream>
#include "scene_node.h"

struct renderer;
struct script;
struct transform;


namespace serial {
    void serialise(std::ostream& os, const renderer& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const script& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const transform& obj, const scene_node* sc, int indt);
}

#endif

