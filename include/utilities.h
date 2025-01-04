#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>

#define DIFFUSE_TEX_UNIT GL_TEXTURE0
#define SPECULAR_TEX_UNIT GL_TEXTURE1

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

void display_gl_version_info();

void clear_gl_errors();

void process_gl_errors(const char* fn_call, int line_no);

#define gl_error_check(fn_call) clear_gl_errors(); fn_call; process_gl_errors(#fn_call, __LINE__);
#define gl_error_check_barrier process_gl_errors("none", __LINE__);

std::string load_from_file(const std::string& file_name);

#endif