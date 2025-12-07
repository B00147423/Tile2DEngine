#pragma once

#include <vector>
#include <string>

struct TextureData {
    int width = 0;
    int height = 0;
    int channels = 0;
    std::vector<unsigned char> pixels;
    std::string filepath;

    bool LoadFromFile(const std::string& path);
    void FreeCPUData();
};

class TextureLoader {
public:
    static TextureData Load(const std::string& path);
    static bool IsPowerOfTwo(int value);
    static TextureData GenerateMipmapsCPU(const TextureData& source);
};

