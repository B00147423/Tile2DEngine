#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

/**
 * Load file: Reads the entire contents of a text file into a string.
 * Opens the file, reads all content into a stringstream, and returns it as a string.
 * Returns empty string if file cannot be opened. Used for loading shader source code.
 */
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

/**
 * Compile stage: Compiles a single shader stage (vertex or fragment).
 * Creates a shader object, uploads source code, compiles it, and checks for errors.
 * Prints compilation errors if any. Returns the shader ID (0 on failure).
 * The caller is responsible for deleting the shader after linking.
 */
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

/**
 * Constructor: Loads, compiles, and links vertex and fragment shaders from files.
 * Reads both shader files, compiles them separately, links them into a program,
 * and stores the OpenGL program ID. Prints errors if compilation or linking fails.
 * The shader stages are deleted after linking (OpenGL keeps the program).
 */
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

/**
 * Destructor: Deletes the OpenGL shader program to prevent memory leaks.
 * Only deletes if id != 0 (handles moved-from objects safely).
 */
Shader::~Shader() {
    if (id != 0) {
        glDeleteProgram(id);
    }
}

/**
 * Move constructor: Transfers ownership of the OpenGL program ID from another Shader.
 * Sets other.id to 0 to prevent double deletion. Used when returning Shader from functions.
 */
Shader::Shader(Shader&& other) noexcept
    : id(other.id)
{
    other.id = 0; // Transfer ownership, prevent double deletion
}

/**
 * Move assignment: Transfers ownership, deleting the current program first if it exists.
 * Prevents self-assignment, deletes old program, transfers ID, and nullifies other.id.
 */
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

/**
 * Use: Activates this shader program for rendering.
 * Calls glUseProgram(id) to make this shader active. All subsequent draw calls
 * will use this shader until another shader is used.
 */
void Shader::use() const {
    glUseProgram(id);
}

/**
 * Set matrix4 uniform: Uploads a 4x4 matrix to a shader uniform.
 * Looks up the uniform location by name and uploads the matrix data.
 * Used for MVP matrices, model matrices, etc.
 */
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

/**
 * Set vec3 uniform: Uploads a 3-component vector to a shader uniform.
 * Looks up uniform location and uploads x, y, z components.
 * Used for colors, positions, directions, etc.
 */
void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3f(glGetUniformLocation(id, name.c_str()), vec.x, vec.y, vec.z);
}

/**
 * Set int uniform: Uploads an integer value to a shader uniform.
 * Looks up uniform location and uploads the integer.
 * Used for texture unit indices, flags, etc.
 */
void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
