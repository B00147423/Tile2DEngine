#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

std::string Shader::loadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to load shader file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileStage(unsigned int type, const std::string& src) {
    GLuint shader = glCreateShader(type);
    const char* csrc = src.c_str();
    glShaderSource(shader, 1, &csrc, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << std::endl;
    }

    return shader;
}

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
    std::string vertSrc = loadFile(vertPath);
    std::string fragSrc = loadFile(fragPath);

    GLuint vs = compileStage(GL_VERTEX_SHADER, vertSrc);
    GLuint fs = compileStage(GL_FRAGMENT_SHADER, fragSrc);

    id = glCreateProgram();
    glAttachShader(id, vs);
    glAttachShader(id, fs);
    glLinkProgram(id);

    GLint success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(id, 512, nullptr, log);
        std::cerr << "Shader linking error: " << log << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);
}


Shader::~Shader() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

Shader::Shader(Shader&& other) noexcept
    : id(other.id)
{
    other.id = 0; // Transfer ownership, prevent double deletion
}
Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        // Delete current program if it exists
        if (id != 0) {
            glDeleteProgram(id);
        }
        // Transfer ownership
        id = other.id;
        other.id = 0; // Prevent double deletion
    }
    return *this;
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
