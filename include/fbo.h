#ifndef DIRECTIONAL_SHADOW_MAP_H
#define DIRECTIONAL_SHADOW_MAP_H

#include <glad/glad.h>

#define DEFAULT_SHADOW_MAP_WIDTH 1920 * 2
#define DEFAULT_SHADOW_MAP_HEIGHT 1920 * 2

#define DEFAULT_REFRACTION_MAP_WIDTH 1280
#define DEFAULT_REFRACTION_MAP_HEIGHT 720

#define DEFAULT_REFLECTION_MAP_WIDTH 320
#define DEFAULT_REFLECTION_MAP_HEIGHT 180

struct fbo {
    int m_pixel_width { DEFAULT_SHADOW_MAP_WIDTH };
    int m_pixel_height { DEFAULT_SHADOW_MAP_HEIGHT };

    GLuint m_fbo {};
    GLuint m_depth_texture {};
    GLuint m_color_texture {};

    bool m_depth_attachment { false };
    bool m_color_attachment { false };
    
    void initialise(int pixel_width, int pixel_height, bool depth, bool color, bool set_boundaries);
    
    void bind_for_writing();
    void bind_depth_for_reading(GLenum texture_unit);
    void bind_color_for_reading(GLenum texture_unit);
};

#endif