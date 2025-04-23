#include <glad/glad.h>
#include <iostream>

#include "directional_shadow_map.h"
#include "utilities.h"

void directional_shadow_map::initialise(int pixel_width, int pixel_height) {
    m_pixel_width = pixel_width;
    m_pixel_height = pixel_height;

    std::cout << "FBO width / height: " << pixel_width << " / " << pixel_height << std::endl;

    glGenFramebuffers(1, &m_fbo);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, pixel_width, pixel_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat border_colour[4] { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_texture, 0);

    GLenum draw_buffers[1] { GL_NONE };
    glDrawBuffers(1, draw_buffers);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "An error occurred when initialising the shadow map's frame buffer. Error code " << status << std::endl;
        abort();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void directional_shadow_map::bind_for_writing() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_pixel_width, m_pixel_height);
}

void directional_shadow_map::bind_for_reading(GLenum texture_unit) {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}