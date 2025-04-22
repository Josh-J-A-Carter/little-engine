#include <iostream>
#include <fstream>

#include <glad/glad.h>
#include "glm/mat4x4.hpp"

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

std::string replace_all(const std::string& templ, const std::string& remove, const std::string& insert) {

    std::string str { templ };
    std::string::size_type pos = 0;
    while ((pos = str.find(remove, pos)) != std::string::npos) {
        str.replace(pos, remove.size(), insert);
        pos += 1;
    }

    return str;
}

void print_mat(glm::mat4& m) {
    std::cout << m[0][0] << "\t" << m[0][1] << "\t" << m[0][2] << "\t" << m[0][3] << std::endl;
    std::cout << m[1][0] << "\t" << m[1][1] << "\t" << m[1][2] << "\t" << m[1][3] << std::endl;
    std::cout << m[2][0] << "\t" << m[2][1] << "\t" << m[2][2] << "\t" << m[2][3] << std::endl;
    std::cout << m[3][0] << "\t" << m[3][1] << "\t" << m[3][2] << "\t" << m[3][3] << std::endl;
}

void print_vec(glm::vec3& m) {
    std::cout << m[0] << "\t" << m[1] << "\t" << m[2] << "\t" << std::endl;
}

void print_vec(glm::vec4& m) {
    std::cout << m[0] << "\t" << m[1] << "\t" << m[2] << "\t" << m[3] << "\t" << std::endl;
}