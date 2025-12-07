#pragma once

#include "TextureData.h"
#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <future>

class AssetManager {
public:
    /**
     * Init: Initializes the AssetManager (currently does nothing, reserved for future setup).
     * Called once at startup before loading any assets.
     */
    static void Init();
    
    /**
     * Shutdown: Cleans up all GPU textures and CPU data.
     * Deletes all OpenGL texture IDs and clears both CPU and GPU texture maps.
     * Called when the application exits to prevent memory leaks.
     */
    static void Shutdown();

    /**
     * Load texture async: Loads a texture file from disk asynchronously on a background thread.
     * Returns a future that will contain a pointer to TextureData when loading completes.
     * The texture is loaded into CPU memory (pixels array). Thread-safe. Multiple textures
     * can be loaded in parallel. Call UploadAllTexturesToGPU() after all loads complete.
     */
    static std::future<TextureData*> LoadTextureAsync(const std::string& path);

    /**
     * Upload all textures to GPU: Batch uploads all loaded CPU textures to GPU memory.
     * Creates OpenGL texture objects, uploads pixel data, sets texture parameters
     * (CLAMP_TO_EDGE, NEAREST filtering), generates mipmaps, and stores GPU IDs.
     * This is FAST because it's a single batch operation. Must be called from main thread
     * (OpenGL context required). Call after all LoadTextureAsync() futures complete.
     */
    static void UploadAllTexturesToGPU();

    /**
     * Get texture data: Returns a pointer to the CPU-side texture data for a given path.
     * Returns nullptr if texture hasn't been loaded yet. Thread-safe. Used for accessing
     * texture metadata (width, height) or pixel data before GPU upload.
     */
    static TextureData* GetTextureData(const std::string& path);
    
    /**
     * Get GPU handle: Returns the OpenGL texture ID for a given path.
     * Returns 0 if texture hasn't been uploaded to GPU yet. Thread-safe. This is what
     * you use for rendering - bind this ID with glBindTexture(GL_TEXTURE_2D, id).
     */
    static GLuint GetGPUHandle(const std::string& path);

    /**
     * Free CPU data: Frees the pixel data from all loaded textures to save RAM.
     * After GPU upload, the CPU pixel data is no longer needed. This frees that memory
     * while keeping the GPU textures intact. Call after UploadAllTexturesToGPU().
     */
    static void FreeCPUDataForLoadedTextures();

private:
    static std::unordered_map<std::string, TextureData> m_cpuTextures;
    static std::unordered_map<std::string, GLuint> m_gpuTextures;
    static std::vector<std::future<void>> m_loadingFutures;
};

