#pragma once

#include "TextureData.h"
#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <future>

class AssetManager {
public:

    static void Init();
    static void Shutdown();
    static std::future<TextureData*> LoadTextureAsync(const std::string& path);
    static void UploadAllTexturesToGPU();
    static TextureData* GetTextureData(const std::string& path);
    static GLuint GetGPUHandle(const std::string& path);
    static void FreeCPUDataForLoadedTextures();

private:
    static std::unordered_map<std::string, TextureData> m_cpuTextures;
    static std::unordered_map<std::string, GLuint> m_gpuTextures;
    static std::vector<std::future<void>> m_loadingFutures;
};

