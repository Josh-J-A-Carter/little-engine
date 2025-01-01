#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

#include <glad/glad.h>

struct texture {
    private:
        GLenum m_texture_target;
        GLuint m_texture_object;
        const std::string& m_file_name;

    public:
        texture(GLenum texture_target, const std::string& file_name) 
            : m_texture_target { texture_target }
            , m_file_name { file_name } {}

        ~texture() {
            glDeleteTextures(1, &m_texture_object);
        }

        void load();

        void bind(GLenum texture_unit) const;
};

#endif