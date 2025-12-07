#include "SceneSerializer.h"


// --------------------------------------------------
// Save JSON: Saves a scene to disk in human-readable JSON format.
// Writes scene name, grid settings, and all entities (type, position, layer) to a JSON file.
// Useful for debugging and manual editing. Returns false on file write errors.
// --------------------------------------------------
bool SceneSerializer::saveJSON(const Scene& scene, const std::string& path)
{
    json j;

    j["sceneName"] = scene.name;

    j["grid"] = {
        {"cellWidth", scene.grid.cellWidth},
        {"cellHeight", scene.grid.cellHeight},
        {"rows", scene.grid.rows},
        {"cols", scene.grid.cols}
    };

    j["entities"] = json::array();
    for (auto& e : scene.entities)
    {
        j["entities"].push_back({
            {"type", e.type},
            {"x", e.x},
            {"y", e.y},
            {"layer", e.layer}
            });
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << "\n";
        return false;
    }

    file << j.dump(4);
    
    if (file.fail() || file.bad()) {
        std::cerr << "Error occurred while writing JSON file: " << path << "\n";
        return false;
    }

    file.close();
    return true;
}

// --------------------------------------------------
// Load JSON: Loads a scene from a JSON file.
// Reads scene name, grid settings, and entities from JSON. Returns false if file
// doesn't exist or is invalid. Used for debugging and human-readable scene files.
// --------------------------------------------------
bool SceneSerializer::loadJSON(Scene& scene, const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    scene.name = j.value("sceneName", "Untitled");

    scene.grid.cellWidth = j["grid"].value("cellWidth", 16.0f);
    scene.grid.cellHeight = j["grid"].value("cellHeight", 16.0f);
    scene.grid.rows = j["grid"].value("rows", 30);
    scene.grid.cols = j["grid"].value("cols", 30);

    scene.entities.clear();
    for (auto& e : j["entities"])
    {
        Entity ent;
        ent.type = e["type"];
        ent.x = e["x"];
        ent.y = e["y"];
        ent.layer = e["layer"];
        scene.entities.push_back(ent);
    }

    return true;
}


/**
 * Write string: Helper function for binary serialization.
 * Writes a string length (uint32_t) followed by the string data itself.
 * Used internally by saveBinary() to write entity types and scene names.
 */
void SceneSerializer::writeString(std::ofstream& out, const std::string& s)
{
    uint32_t len = (uint32_t)s.size();
    out.write((char*)&len, sizeof(len));
    out.write(s.data(), len);
}

/**
 * Write float: Helper function for binary serialization.
 * Writes a single float value directly to the binary stream.
 * Used for writing entity positions and grid cell dimensions.
 */
void SceneSerializer::writeFloat(std::ofstream& out, float f)
{
    out.write((char*)&f, sizeof(float));
}

/**
 * Write int: Helper function for binary serialization.
 * Writes a single integer value directly to the binary stream.
 * Used for writing entity counts, layers, grid dimensions, and version numbers.
 */
void SceneSerializer::writeInt(std::ofstream& out, int i)
{
    out.write((char*)&i, sizeof(int));
}

// --------------------------------------------------
// Read string: Helper function for binary deserialization.
// Reads a string length (uint32_t) then reads that many characters.
// Used internally by loadBinary() to read entity types and scene names.
// --------------------------------------------------
std::string SceneSerializer::readString(std::ifstream& in)
{
    uint32_t len;
    in.read((char*)&len, sizeof(len));

    std::string s(len, '\0');
    in.read(&s[0], len);
    return s;
}

/**
 * Read float: Helper function for binary deserialization.
 * Reads a single float value directly from the binary stream.
 * Used for reading entity positions and grid cell dimensions.
 */
float SceneSerializer::readFloat(std::ifstream& in)
{
    float f;
    in.read((char*)&f, sizeof(float));
    return f;
}

/**
 * Read int: Helper function for binary deserialization.
 * Reads a single integer value directly from the binary stream.
 * Used for reading entity counts, layers, grid dimensions, and version numbers.
 */
int SceneSerializer::readInt(std::ifstream& in)
{
    int v;
    in.read((char*)&v, sizeof(int));
    return v;
}

// --------------------------------------------------
// Save binary: Saves a scene to disk in fast binary format.
// Writes a version number, scene name, grid settings, and all entities in binary format.
// Much faster than JSON for loading. Used for runtime scene files (.map extension).
// Returns false on file write errors.
// --------------------------------------------------
bool SceneSerializer::saveBinary(const Scene& scene, const std::string& path)
{
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << "\n";
        return false;
    }

    // Version number - allows future changes
    int version = 1;
    writeInt(out, version);

    writeString(out, scene.name);

    writeFloat(out, scene.grid.cellWidth);
    writeFloat(out, scene.grid.cellHeight);
    writeInt(out, scene.grid.rows);
    writeInt(out, scene.grid.cols);

    writeInt(out, (int)scene.entities.size());
    for (auto& e : scene.entities) {
        writeString(out, e.type);
        writeFloat(out, e.x);
        writeFloat(out, e.y);
        writeInt(out, e.layer);
    }

    // Check if write operations succeeded
    if (out.fail() || out.bad()) {
        std::cerr << "Error occurred while writing binary file: " << path << "\n";
        return false;
    }

    out.close();
    return true;
}

// --------------------------------------------------
// Load binary: Loads a scene from a binary file.
// Reads version number (for future compatibility), scene name, grid settings, and entities.
// Much faster than JSON. Returns false if file doesn't exist, version mismatch, or read errors.
// This is the primary loading method for runtime.
// --------------------------------------------------
bool SceneSerializer::loadBinary(Scene& scene, const std::string& path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) return false;

    int version = readInt(in);
    if (version != 1) {
        return false; // future proofing
    }

    scene.name = readString(in);

    scene.grid.cellWidth = readFloat(in);
    scene.grid.cellHeight = readFloat(in);
    scene.grid.rows = readInt(in);
    scene.grid.cols = readInt(in);

    int entityCount = readInt(in);
    scene.entities.clear();
    scene.entities.reserve(entityCount);

    for (int i = 0; i < entityCount; i++) {
        Entity e;
        e.type = readString(in);
        e.x = readFloat(in);
        e.y = readFloat(in);
        e.layer = readInt(in);
        scene.entities.push_back(e);
    }

    return true;
}
