#include <iostream>
#include <fstream>

#include <glad/glad.h>

#include "utilities.h"

void display_gl_version_info() {
    std::cerr << glGetString(GL_VENDOR) << std::endl;
    std::cerr << glGetString(GL_RENDERER) << std::endl;
    std::cerr << glGetString(GL_VERSION) << std::endl;
    std::cerr << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    int texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);

    std::cerr << "Texture units: " << texture_units << std::endl;
}

void clear_gl_errors() {
    while (glGetError() != GL_NO_ERROR) {}
}

void process_gl_errors(const char* fn_call, int line_no) {
    GLenum code = glGetError();
    if (code == GL_NO_ERROR) return;

    std::cerr << "OpenGL error  " << code << "  at line  " << line_no << ", in function call " << fn_call << std::endl;
}

std::string load_from_file(const std::string& file_name) {
    std::string result {};

    std::string line {};

    std::ifstream file { file_name };

    // Make sure the file actually exists
    if (file.is_open() == false) {
        std::cerr << "Fatal: unable to find file \"" << file_name << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }

    while (std::getline(file, line)) {
        result.append(line)
              .append("\n");
    }

    file.close();

    return result;
}
