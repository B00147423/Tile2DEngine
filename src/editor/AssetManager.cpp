#include "AssetManager.h"
#include <glad/glad.h>
#include <iostream>
#include <mutex>

std::unordered_map<std::string, TextureData> AssetManager::m_cpuTextures;
std::unordered_map<std::string, GLuint> AssetManager::m_gpuTextures;
std::vector<std::future<void>> AssetManager::m_loadingFutures;
static std::mutex s_textureMutex;

/**
 * Init: Initializes the AssetManager (currently does nothing, reserved for future setup).
 * Called once at startup before loading any assets.
 */
void AssetManager::Init() {
    
}

/**
 * Shutdown: Cleans up all GPU textures and CPU data.
 * Deletes all OpenGL texture IDs and clears both CPU and GPU texture maps.
 * Called when the application exits to prevent memory leaks.
 */
void AssetManager::Shutdown() {
    // Clean up GPU textures
    for (auto& [path, textureID] : m_gpuTextures) {
        glDeleteTextures(1, &textureID);
    }
    m_gpuTextures.clear();
    m_cpuTextures.clear();
}

/**
 * Load texture async: Loads a texture file from disk asynchronously on a background thread.
 * Returns a future that will contain a pointer to TextureData when loading completes.
 * The texture is loaded into CPU memory (pixels array). Thread-safe. Multiple textures
 * can be loaded in parallel. Call UploadAllTexturesToGPU() after all loads complete.
 */
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

/**
 * Upload all textures to GPU: Batch uploads all loaded CPU textures to GPU memory.
 * Creates OpenGL texture objects, uploads pixel data, sets texture parameters
 * (CLAMP_TO_EDGE, NEAREST filtering), generates mipmaps, and stores GPU IDs.
 * This is FAST because it's a single batch operation. Must be called from main thread
 * (OpenGL context required). Call after all LoadTextureAsync() futures complete.
 */
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

/**
 * Get texture data: Returns a pointer to the CPU-side texture data for a given path.
 * Returns nullptr if texture hasn't been loaded yet. Thread-safe. Used for accessing
 * texture metadata (width, height) or pixel data before GPU upload.
 */
TextureData* AssetManager::GetTextureData(const std::string& path) {
    std::lock_guard<std::mutex> lock(s_textureMutex);
    auto it = m_cpuTextures.find(path);
    return (it != m_cpuTextures.end()) ? &it->second : nullptr;
}

/**
 * Get GPU handle: Returns the OpenGL texture ID for a given path.
 * Returns 0 if texture hasn't been uploaded to GPU yet. Thread-safe. This is what
 * you use for rendering - bind this ID with glBindTexture(GL_TEXTURE_2D, id).
 */
GLuint AssetManager::GetGPUHandle(const std::string& path) {
    std::lock_guard<std::mutex> lock(s_textureMutex);
    auto it = m_gpuTextures.find(path);
    return (it != m_gpuTextures.end()) ? it->second : 0;
}

/**
 * Free CPU data: Frees the pixel data from all loaded textures to save RAM.
 * After GPU upload, the CPU pixel data is no longer needed. This frees that memory
 * while keeping the GPU textures intact. Call after UploadAllTexturesToGPU().
 */
void AssetManager::FreeCPUDataForLoadedTextures() {
    std::lock_guard<std::mutex> lock(s_textureMutex);
    for (auto& [path, texture] : m_cpuTextures) {
        texture.FreeCPUData();
    }
    std::cout << "Freed all CPU texture memory\n";
}

