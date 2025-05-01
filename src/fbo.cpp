#include <glad/glad.h>
#include <iostream>

#include "fbo.h"
#include "utilities.h"

void fbo::initialise(int pixel_width, int pixel_height, bool depth, bool color, bool set_boundaries) {
    m_pixel_width = pixel_width;
    m_pixel_height = pixel_height;

    m_depth_attachment = depth;
    m_color_attachment = color;

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    if (depth) {
        glGenTextures(1, &m_depth_texture);
        glBindTexture(GL_TEXTURE_2D, m_depth_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, pixel_width, pixel_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        if (set_boundaries) {
            GLfloat border_colour[4] { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);
        }
    
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);
    } else {
        glGenRenderbuffers(1, &m_depth_texture);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_texture);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, pixel_width, pixel_height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_texture);
    }

    if (color) {
        glGenTextures(1, &m_color_texture);
        glBindTexture(GL_TEXTURE_2D, m_color_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixel_width, pixel_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        // if (set_boundaries) {
        //     GLfloat border_colour[4] { 1.0f, 1.0f, 1.0f, 1.0f };
        //     glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_colour);
        // }
    
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);
    } else {
        GLenum draw_buffers[1] { GL_NONE };
        glDrawBuffers(1, draw_buffers);
        glReadBuffer(GL_NONE);
    }


    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "An error occurred when initialising the shadow map's frame buffer. Error code " << status << std::endl;
        abort();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void fbo::bind_for_writing() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_pixel_width, m_pixel_height);
}

void fbo::bind_depth_for_reading(GLenum texture_unit) {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
}

void fbo::bind_color_for_reading(GLenum texture_unit) {
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);
}