#include "Editor.h"
#include "SceneSerializer.h"

//
void Editor::newScene(const std::string& name) {
    currentScene.name = name;
    currentScene.grid = { cellWidth, cellHeight, 20, 30 };
    currentScene.entities.clear();
}
//
//void Editor::loadScene(const std::string& path) {
//    std::ifstream file(path);
//    if (!file.is_open()) {
//        std::cerr << "Failed to open " << path << "\n";
//        return;
//    }
//    json j;
//    file >> j;
//
//    // Name
//    if (j.contains("sceneName") && j["sceneName"].is_string() && !j["sceneName"].get<std::string>().empty()) {
//        currentScene.name = j["sceneName"];
//    }
//    else {
//        currentScene.name = fs::path(path).stem().string();
//    }
//
//    // Grid + Camera
//    cellWidth = j["grid"]["cellWidth"];
//    cellHeight = j["grid"]["cellHeight"];
//    gameViewWidth = j["camera"]["width"];
//    gameViewHeight = j["camera"]["height"];
//    cameraX = j["camera"]["x"];
//    cameraY = j["camera"]["y"];
//    zoom = j["camera"]["zoom"];
//
//    m_camera.setPosition(cameraX, cameraY);
//    m_camera.setZoom(zoom);
//
//    // Entities
//    currentScene.entities.clear();
//    for (auto& e : j["entities"]) {
//        Entity ent;
//        ent.type = e["type"];
//        ent.x = e["x"];
//        ent.y = e["y"];
//        ent.layer = e.value("layer", 0);  // LOAD LAYER (default 0)
//        currentScene.entities.push_back(ent);
//    }
//
//    currentScene.path = path;
//    std::cout << "Loaded scene: " << currentScene.name << " (" << path << ")\n";
//}
//
//void Editor::saveScene(const std::string& path) {
//    currentScene.name = fs::path(path).stem().string();
//    currentScene.path = path;
//
//    json j;
//    j["sceneName"] = currentScene.name;
//    j["grid"] = {
//        {"cellWidth",  cellWidth},
//        {"cellHeight", cellHeight},
//        {"rows", currentScene.grid.rows},
//        {"cols", currentScene.grid.cols}
//    };
//    j["camera"] = {
//        {"x", cameraX},
//        {"y", cameraY},
//        {"zoom", zoom},
//        {"width",  gameViewWidth},
//        {"height", gameViewHeight}
//    };
//
//    j["entities"] = json::array();
//    for (auto& e : currentScene.entities) {
//        j["entities"].push_back({
//            {"type",  e.type},
//            {"x",     e.x},
//            {"y",     e.y},
//            {"layer", e.layer} 
//            });
//    }
//
//    std::ofstream file(path);
//    if (!file.is_open()) {
//        std::cerr << "❌ Failed to save scene to " << path << "\n";
//        return;
//    }
//    file << j.dump(4);
//    file.close();
//
//    std::cout << "Saved scene: " << currentScene.name << " → " << path << "\n";
//}


void Editor::saveScene(const std::string& path) {
    if (!SceneSerializer::saveBinary(currentScene, path)) {
        std::cerr << "Failed to save binary scene: " << path << "\n";
        return;
    }

    currentScene.path = path;
    std::cout << "Saved binary scene: " << currentScene.name << " → " << path << "\n";
}


void Editor::loadScene(const std::string& path) {
    if (!SceneSerializer::loadBinary(currentScene, path)) {
        std::cerr << "Failed to load binary scene: " << path << "\n";
        return;
    }

    currentScene.path = path;
    std::cout << "Loaded binary scene: " << currentScene.name << " (" << path << ")\n";
}
