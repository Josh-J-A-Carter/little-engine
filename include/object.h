#ifndef OBJECT_H
#define OBJECT_H

#include "transform.h"
#include "mesh.h"

struct object {
    transform transform;
    mesh* mesh;
};

#endif