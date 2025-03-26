#ifndef DIRECTIONAL_SHADOW_MAP_H
#define DIRECTIONAL_SHADOW_MAP_H

#include <glad/glad.h>

#define DEFAULT_SHADOW_MAP_WIDTH 1920
#define DEFAULT_SHADOW_MAP_HEIGHT 1080

struct directional_shadow_map {
    private:
        int m_pixel_width { DEFAULT_SHADOW_MAP_WIDTH };
        int m_pixel_height { DEFAULT_SHADOW_MAP_HEIGHT };

        GLuint m_fbo {};
        GLuint m_texture {};

        
    public:
        void initialise(int pixel_width, int pixel_height);
        void initialise() {
            initialise(m_pixel_width, m_pixel_height);
        }
        
        void bind_for_writing();
        void bind_for_reading(GLenum texture_unit);
};

#endif