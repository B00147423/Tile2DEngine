#include "AssetManager.h"
#include <glad/glad.h>
#include <iostream>
#include <mutex>

std::unordered_map<std::string, TextureData> AssetManager::m_cpuTextures;
std::unordered_map<std::string, GLuint> AssetManager::m_gpuTextures;
std::vector<std::future<void>> AssetManager::m_loadingFutures;
static std::mutex s_textureMutex;

void AssetManager::Init() {
    
}

void AssetManager::Shutdown() {
    // Clean up GPU textures
    for (auto& [path, textureID] : m_gpuTextures) {
        glDeleteTextures(1, &textureID);
    }
    m_gpuTextures.clear();
    m_cpuTextures.clear();
}

std::future<TextureData*> AssetManager::LoadTextureAsync(const std::string& path) {
    // This runs on a separate thread!
    return std::async(std::launch::async, [path]() -> TextureData* {
        std::lock_guard<std::mutex> lock(s_textureMutex);

        // Check if already loaded
        auto it = m_cpuTextures.find(path);
        if (it != m_cpuTextures.end()) {
            return &it->second;
        }

        // Load new texture
        auto& texture = m_cpuTextures[path];
        if (texture.LoadFromFile(path)) {
            std::cout << "Loaded texture (CPU): " << path << "\n";
            return &texture;
        }

        // Failed to load, remove from map
        m_cpuTextures.erase(path);
        return nullptr;
        });
}

void AssetManager::UploadAllTexturesToGPU() {
    std::lock_guard<std::mutex> lock(s_textureMutex);

    std::cout << "Batch uploading " << m_cpuTextures.size() << " textures to GPU...\n";

    for (auto& [path, textureData] : m_cpuTextures) {
        // Skip if already uploaded
        if (m_gpuTextures.find(path) != m_gpuTextures.end()) {
            continue;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Upload to GPU (this is the only GL call per texture now)
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA8,
            textureData.width, textureData.height,
            0, GL_RGBA, GL_UNSIGNED_BYTE,
            textureData.pixels.data()
        );

        // Generate mipmaps on GPU (fast)
        glGenerateMipmap(GL_TEXTURE_2D);

        m_gpuTextures[path] = textureID;
        std::cout << "Uploaded to GPU: " << path << " [ID: " << textureID << "]\n";
    }

    std::cout << "Batch upload complete!\n";
}

TextureData* AssetManager::GetTextureData(const std::string& path) {
    std::lock_guard<std::mutex> lock(s_textureMutex);
    auto it = m_cpuTextures.find(path);
    return (it != m_cpuTextures.end()) ? &it->second : nullptr;
}

GLuint AssetManager::GetGPUHandle(const std::string& path) {
    std::lock_guard<std::mutex> lock(s_textureMutex);
    auto it = m_gpuTextures.find(path);
    return (it != m_gpuTextures.end()) ? it->second : 0;
}

void AssetManager::FreeCPUDataForLoadedTextures() {
    std::lock_guard<std::mutex> lock(s_textureMutex);
    for (auto& [path, texture] : m_cpuTextures) {
        texture.FreeCPUData();
    }
    std::cout << "Freed all CPU texture memory\n";
}

