#include "SceneSerializer.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

// --------------------------------------------------
// JSON SAVE
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
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

// --------------------------------------------------
// JSON LOAD
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


// BINARY HELPER WRITE FUNCTIONS--
void SceneSerializer::writeString(std::ofstream& out, const std::string& s)
{
    uint32_t len = (uint32_t)s.size();
    out.write((char*)&len, sizeof(len));
    out.write(s.data(), len);
}

void SceneSerializer::writeFloat(std::ofstream& out, float f)
{
    out.write((char*)&f, sizeof(float));
}

void SceneSerializer::writeInt(std::ofstream& out, int i)
{
    out.write((char*)&i, sizeof(int));
}

// --------------------------------------------------
// BINARY HELPER READ FUNCTIONS
// --------------------------------------------------
std::string SceneSerializer::readString(std::ifstream& in)
{
    uint32_t len;
    in.read((char*)&len, sizeof(len));

    std::string s(len, '\0');
    in.read(&s[0], len);
    return s;
}

float SceneSerializer::readFloat(std::ifstream& in)
{
    float f;
    in.read((char*)&f, sizeof(float));
    return f;
}

int SceneSerializer::readInt(std::ifstream& in)
{
    int v;
    in.read((char*)&v, sizeof(int));
    return v;
}

// --------------------------------------------------
// BINARY SAVE
// --------------------------------------------------
bool SceneSerializer::saveBinary(const Scene& scene, const std::string& path)
{
    std::ofstream out(path, std::ios::binary);
    if (!out.is_open()) return false;

    // Version number � allows future changes
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

    return true;
}

// --------------------------------------------------
// BINARY LOAD
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
