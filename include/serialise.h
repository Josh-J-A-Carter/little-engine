#ifndef SERIALISE_H
#define SERIALISE_H

#include <ostream>
#include <fstream>
#include <vector>
#include <string>

#include "utilities.h"
#include "arena.h"


// Deserialisation

void read_scene_from_file(arena& arena, std::string filename);


// Serialisation

inline std::string indent(int n) {
    return std::string(2 * n, ' ');
}

template<typename T>
struct TypeParseTraits;

#define REGISTER_PARSE_TYPE(X) template <> struct TypeParseTraits<X> \
    { static const char* name; } ; inline const char* TypeParseTraits<X>::name = #X;


REGISTER_PARSE_TYPE(float)
inline void serialise(std::ostream& os, const float s, int indt = 0) {
    os << s;
}

REGISTER_PARSE_TYPE(int)
inline void serialise(std::ostream& os, const int s, int indt = 0) {
    os << s;
}

template <typename T>
void serialise(std::ostream& os, const std::vector<T> s, int indt = 0) {
    os << "[\n";

    int i_inner = indt + 1;
    for (int i = 0 ; i < s.size() ; i += 1) {
        os << indent(i_inner);
        serialise(os, s[i], i_inner + 1);

        os << (i < s.size() - 1 ? ",\n" : "\n");
    }

    os << indent(indt) << "]\n";
}

#define REPORT(SERIALISER, FIELD) SERIALISER.report(#FIELD, SERIALISER.object.FIELD);

template <typename S>
struct serialiser {
    private:
        std::ostream& os;
        int indt { 0 };
                
    public:
        const S& object;

        serialiser(std::ostream& os, const S& object, int indt) : os { os }, object { object }, indt { indt } {
            os << "{\n";
            os << indent(indt + 1) << "type: " << TypeParseTraits<S>::name;
        }

        ~serialiser() {
            os << "\n" << indent(indt) << "}\n";
        }

        template <typename T>
        inline void report(std::string field_name, T field_value) {
            os << ",\n";
            os << indent(indt + 1) << field_name << ": ";
            serialise(os, field_value, indt + 2);
        }
};

#endif