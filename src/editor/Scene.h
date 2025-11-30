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
};
