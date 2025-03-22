#ifndef PARSE_DECLARATIONS_H
#define PARSE_DECLARATIONS_H

#include <ostream>
#include "scene_node.h"

struct camera;
struct directional_light;
struct light;
struct point_light;
struct renderer;
struct script;
struct transform;


namespace serial {
    void serialise(std::ostream& os, const camera& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const directional_light& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const light& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const point_light& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const renderer& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const script& obj, const scene_node* sc, int indt);
    void serialise(std::ostream& os, const transform& obj, const scene_node* sc, int indt);
}

#endif

