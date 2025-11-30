#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    Shader() = default;
    Shader(const std::string& vertPath, const std::string& fragPath);

    void use() const;
    void setMat4(const std::string&, const glm::mat4&) const;
    void setVec3(const std::string&, const glm::vec3&) const;
    void setInt(const std::string&, int value) const;

    unsigned int id = 0;

private:
    std::string loadFile(const std::string& path);
    unsigned int compileStage(unsigned int type, const std::string& src);
};
