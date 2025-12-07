#pragma once
#include <string>
#include <fstream>
#include "Scene.h"
#include <nlohmann/json.hpp>
#include <iostream>
using json = nlohmann::json;
class SceneSerializer {
public:
    /**
     * Save JSON: Saves a scene to disk in human-readable JSON format.
     * Writes scene name, grid settings, and all entities (type, position, layer) to a JSON file.
     * Useful for debugging and manual editing. Returns false on file write errors.
     */
    static bool saveJSON(const Scene& scene, const std::string& path);
    
    /**
     * Load JSON: Loads a scene from a JSON file.
     * Reads scene name, grid settings, and entities from JSON. Returns false if file
     * doesn't exist or is invalid. Used for debugging and human-readable scene files.
     */
    static bool loadJSON(Scene& scene, const std::string& path);

    /**
     * Save binary: Saves a scene to disk in fast binary format.
     * Writes a version number, scene name, grid settings, and all entities in binary format.
     * Much faster than JSON for loading. Used for runtime scene files (.map extension).
     * Returns false on file write errors.
     */
    static bool saveBinary(const Scene& scene, const std::string& path);
    
    /**
     * Load binary: Loads a scene from a binary file.
     * Reads version number (for future compatibility), scene name, grid settings, and entities.
     * Much faster than JSON. Returns false if file doesn't exist, version mismatch, or read errors.
     * This is the primary loading method for runtime.
     */
    static bool loadBinary(Scene& scene, const std::string& path);

private:
    static void writeString(std::ofstream& out, const std::string& s);
    static void writeFloat(std::ofstream& out, float f);
    static void writeInt(std::ofstream& out, int i);

    static std::string readString(std::ifstream& in);
    static float readFloat(std::ifstream& in);
    static int readInt(std::ifstream& in);
};
