#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>

void display_gl_version_info();

void clear_gl_errors();

void process_gl_errors(const char* fn_call, int line_no);

#define gl_error_check(fn_call) clear_gl_errors(); fn_call; process_gl_errors(#fn_call, __LINE__);

std::string load_from_file(const std::string& file_name);

#endif