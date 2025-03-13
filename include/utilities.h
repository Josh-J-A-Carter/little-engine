#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <variant>
#include <algorithm> 
#include <cctype>
#include <locale>

#define DIFFUSE_TEX_UNIT GL_TEXTURE0
#define DIFFUSE_TEX_UNIT_INDEX 0
#define SPECULAR_TEX_UNIT GL_TEXTURE1
#define SPECULAR_TEX_UNIT_INDEX 1

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

void display_gl_version_info();

void clear_gl_errors();

void process_gl_errors(const char* fn_call, int line_no);

#define gl_error_check(fn_call) clear_gl_errors(); fn_call; process_gl_errors(#fn_call, __LINE__);
#define gl_error_check_barrier process_gl_errors("none", __LINE__);

std::string load_from_file(const std::string& file_name);

inline void clearspace(std::string &s) {
    std::string out {};
    for ( char ch : s ) {
        if (std::isspace(ch)) continue;

        out += ch;
    }

    s.assign(out);
}

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

template <typename S, typename T>
using option = std::variant<S, T>;

struct error {
    std::string message;
};

#endif