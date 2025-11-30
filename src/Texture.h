#pragma once
#include <glad/glad.h>
#include <string>

class Texture {
public:
    GLuint id = 0;
    int width = 0, height = 0;

    explicit Texture(const std::string& path);
    void bind() const { glBindTexture(GL_TEXTURE_2D, id); }
};
