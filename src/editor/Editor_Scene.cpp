#include "Editor.h"
#include "SceneSerializer.h"

/**
 * New scene: Creates a blank new scene with the given name.
 * Sets scene name, initializes grid settings from current cellWidth/cellHeight,
 * clears all entities, and sets entitiesNeedSorting=false (empty list doesn't need sorting).
 * Called when user clicks "New Scene" or from menu.
 */
void Editor::newScene(const std::string& name) {
    currentScene.name = name;
    currentScene.grid = { cellWidth, cellHeight, 20, 30 };
    currentScene.entities.clear();
    currentScene.gameViewWidth = gameViewWidth;   // Initialize from editor
    currentScene.gameViewHeight = gameViewHeight;  // Initialize from editor
    entitiesNeedSorting = false; // No need to sort empty list
}

/**
 * Save scene: Saves the current scene to disk in both binary (.map) and JSON (.json) formats.
 * Binary format is fast for runtime loading, JSON is human-readable for debugging.
 * Uses SceneSerializer to do the actual file writing. Sets currentScene.path after
 * successful save. Prints errors if file operations fail. Called when user saves.
 */
void Editor::saveScene(const std::string& path) {
    // Sync current editor values to scene before saving
    currentScene.gameViewWidth = gameViewWidth;
    currentScene.gameViewHeight = gameViewHeight;
    
    // Binary for runtime (fast)
    if (!SceneSerializer::saveBinary(currentScene, path + ".map")) {
        std::cerr << "Failed to save binary scene: " << path + ".map" << "\n";
        return;
    }

    // JSON for debugging readable
    if (!SceneSerializer::saveJSON(currentScene, path + ".json")) {
        std::cerr << "Failed to save JSON scene: " << path + ".json" << "\n";
        return;
    }

    currentScene.path = path + ".map";
    std::cout << "Saved scene: " << currentScene.name << " → " << path << "\n";
}

/**
 * Load scene: Loads a scene from a binary .map file.
 * Uses SceneSerializer to read the binary file, populates currentScene with the
 * loaded data (name, grid settings, entities), sets the scene path, and marks
 * entities for sorting. Prints error if file doesn't exist or is invalid.
 * Called when user opens a scene from the dialog.
 */
void Editor::loadScene(const std::string& path) {
    if (!SceneSerializer::loadBinary(currentScene, path)) {
        std::cerr << "Failed to load binary scene: " << path << "\n";
        return;
    }

    currentScene.path = path;

    // Set the name from the path (if the serializer didn't do it)
    currentScene.name = fs::path(path).stem().string();

    // Sync loaded scene values to editor UI
    gameViewWidth = currentScene.gameViewWidth;
    gameViewHeight = currentScene.gameViewHeight;
    
    // Update camera virtual size when loading scene (not during editing)
    m_camera.setVirtualSize(gameViewWidth, gameViewHeight);

    entitiesNeedSorting = true;
    cachedTexturePaths.clear();

    std::cout << "Loaded scene: " << currentScene.name << " (" << path << ")\n";
}
