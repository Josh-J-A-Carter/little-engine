#include <iostream>

#include <glad/glad.h>
#include "stb_image.h"

#include "texture.h"

void texture::load() {
    stbi_set_flip_vertically_on_load(true);

    int width, height, num_channels;
    unsigned char* image_data = stbi_load(m_file_name.c_str(), &width, &height, &num_channels, 0);

    if (!image_data) {
        std::cerr << "Unable to load image file \"" << m_file_name << "\"." << std::endl;
        std::cerr << "Error message: " << stbi_failure_reason() << std::endl;
        exit(EXIT_FAILURE);
    }

    glGenTextures(1, &m_texture_object);
    glBindTexture(m_texture_target, m_texture_object);

    if (m_texture_target != GL_TEXTURE_2D) {
        std::cerr << "Texture type unsupported; only 2D textures are possible." << std::endl;
        exit(EXIT_FAILURE);
    }
    
    if (num_channels == 1) {
        glTexImage2D(m_texture_target, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image_data);
    }

    else if (num_channels == 3) {
        glTexImage2D(m_texture_target, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    }

    else if (num_channels == 4) {
        glTexImage2D(m_texture_target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    }

    else {
        std::cerr << "Texture type unsupported; unexpected number of color channels." << std::endl;
        exit(EXIT_FAILURE);
    }

    glTexParameterf(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(m_texture_target, 0);

    free(image_data);
}

void texture::bind(GLenum texture_unit) const {
    glActiveTexture(texture_unit);
    glBindTexture(m_texture_target, m_texture_object);
}
