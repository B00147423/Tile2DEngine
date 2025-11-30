#pragma once
#include <string>

struct Entity {
    int id;             // ✅ add this
    std::string type = "";
    float x = 0.0f;
    float y = 0.0f;
    int layer = 0;
};

