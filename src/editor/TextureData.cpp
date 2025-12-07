#include "TextureData.h"
#include "../../vendor/stb_image.h"
#include <iostream>

bool TextureData::LoadFromFile(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << path << "\n";
        return false;
    }

    // Copy to vector (CPU memory)
    size_t dataSize = width * height * 4;
    pixels.assign(data, data + dataSize);

    stbi_image_free(data);
    filepath = path;
    channels = 4; // Force RGBA
    return true;
}

void TextureData::FreeCPUData() {
    pixels.clear();
    pixels.shrink_to_fit();
}

TextureData TextureLoader::Load(const std::string& path) {
    TextureData texture;
    texture.LoadFromFile(path);
    return texture;
}

bool TextureLoader::IsPowerOfTwo(int value) {
    return value > 0 && (value & (value - 1)) == 0;
}

TextureData TextureLoader::GenerateMipmapsCPU(const TextureData& source) {
    // Placeholder for CPU-side mipmap generation if needed
    // For now, we'll let OpenGL generate mipmaps on GPU
    return source;
}

