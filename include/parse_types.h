#ifndef PARSE_TYPES_H
#define PARSE_TYPES_H

enum class scene_node_type {
    empty,

    // Dynamic generation
    camera,
    directional_light,
    light,
    point_light,
    renderer,
    script,
    transform,
};

#endif

