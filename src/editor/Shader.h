#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    /**
     * Default constructor: Creates an empty shader with id=0.
     * Used for move assignment or delayed initialization.
     */
    Shader() = default;
    
    /**
     * Constructor: Loads, compiles, and links vertex and fragment shaders from files.
     * Reads both shader files, compiles them separately, links them into a program,
     * and stores the OpenGL program ID. Prints errors if compilation or linking fails.
     * The shader stages are deleted after linking (OpenGL keeps the program).
     */
    Shader(const std::string& vertPath, const std::string& fragPath);
    
    /**
     * Destructor: Deletes the OpenGL shader program to prevent memory leaks.
     * Only deletes if id != 0 (handles moved-from objects safely).
     */
    ~Shader();

    // Disable copy constructor and assignment to prevent double deletion
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Enable move semantics for efficient resource transfer
    /**
     * Move constructor: Transfers ownership of the OpenGL program ID from another Shader.
     * Sets other.id to 0 to prevent double deletion. Used when returning Shader from functions.
     */
    Shader(Shader&& other) noexcept;
    
    /**
     * Move assignment: Transfers ownership, deleting the current program first if it exists.
     * Prevents self-assignment, deletes old program, transfers ID, and nullifies other.id.
     */
    Shader& operator=(Shader&& other) noexcept;

    /**
     * Use: Activates this shader program for rendering.
     * Calls glUseProgram(id) to make this shader active. All subsequent draw calls
     * will use this shader until another shader is used.
     */
    void use() const;
    
    /**
     * Set matrix4 uniform: Uploads a 4x4 matrix to a shader uniform.
     * Looks up the uniform location by name and uploads the matrix data.
     * Used for MVP matrices, model matrices, etc.
     */
    void setMat4(const std::string&, const glm::mat4&) const;
    
    /**
     * Set vec3 uniform: Uploads a 3-component vector to a shader uniform.
     * Looks up uniform location and uploads x, y, z components.
     * Used for colors, positions, directions, etc.
     */
    void setVec3(const std::string&, const glm::vec3&) const;
    
    /**
     * Set int uniform: Uploads an integer value to a shader uniform.
     * Looks up uniform location and uploads the integer.
     * Used for texture unit indices, flags, etc.
     */
    void setInt(const std::string&, int value) const;

    unsigned int id = 0;

private:
    std::string loadFile(const std::string& path);
    unsigned int compileStage(unsigned int type, const std::string& src);
};
