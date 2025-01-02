#include <iostream>

#include <glad/glad.h>
#include "stb_image.h"

#include "texture.h"

void texture::load() {
    stbi_set_flip_vertically_on_load(true);

    int width, height, bits_per_pixel;
    unsigned char* image_data = stbi_load(m_file_name.c_str(), &width, &height, &bits_per_pixel, 0);

    if (!image_data) {
        std::cerr << "Unable to load image file \"" << m_file_name << "\"." << std::endl;
        std::cerr << "Error message: " << stbi_failure_reason() << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cerr << "Loaded image data" << std::endl;

    glGenTextures(1, &m_texture_object);
    glBindTexture(m_texture_target, m_texture_object);

    if (m_texture_target != GL_TEXTURE_2D) {
        std::cerr << "Texture type unsupported." << std::endl;
        exit(EXIT_FAILURE);
    }

    glTexImage2D(m_texture_target, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

    glTexParameterf(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(m_texture_target, 0);

    free(image_data);
}

void texture::bind(GLenum texture_unit) const {
    glActiveTexture(texture_unit);
    glBindTexture(m_texture_target, m_texture_object);
}
