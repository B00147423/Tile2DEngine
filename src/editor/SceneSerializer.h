#pragma once
#include <string>
#include <fstream>
#include "Scene.h"

class SceneSerializer {
public:
    static bool saveJSON(const Scene& scene, const std::string& path);
    static bool loadJSON(Scene& scene, const std::string& path);

    static bool saveBinary(const Scene& scene, const std::string& path);
    static bool loadBinary(Scene& scene, const std::string& path);

private:
    static void writeString(std::ofstream& out, const std::string& s);
    static void writeFloat(std::ofstream& out, float f);
    static void writeInt(std::ofstream& out, int i);

    static std::string readString(std::ifstream& in);
    static float readFloat(std::ifstream& in);
    static int readInt(std::ifstream& in);
};
