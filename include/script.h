#ifndef SCRIPT_H
#define SCRIPT_H

#include <ostream>
#include <iostream>

#include "serialise.h"

struct script {
    int a { 0 };
    int b { 100 };

    float c { 400 };

    std::vector<int> r { 1, 2, 3, 4 };
};

REGISTER_PARSE_TYPE(script);
inline void serialise(std::ostream& os, const script& obj, int indt = 0) {
    std::cout << "begin serialisation" << std::endl;

    serialiser<script> sr = { os, obj, indt };

    REPORT(sr, a)
    REPORT(sr, b)
    REPORT(sr, c)
    REPORT(sr, r)
}

#endif