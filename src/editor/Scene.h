#pragma once
#include <string>
#include <vector>
#include "Entity.h"
#include "GridSettings.h"

struct Scene {
    std::string name;
    GridSettings grid;
    std::vector<Entity> entities;
    std::string path;
    float gameViewWidth;   // Camera virtual width (red square width)
    float gameViewHeight;  // Camera virtual height (red square height)
};
