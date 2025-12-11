# How to Implement Game Mode Yourself

This guide shows you step-by-step how to add Game mode to your Tile2DEngine.

## Step 1: Add Game View Dimensions to Scene

**File: `src/editor/Scene.h`**
```cpp
struct Scene {
    std::string name;
    GridSettings grid;
    std::vector<Entity> entities;
    std::string path;
    float gameViewWidth = 2000.0f;   // ADD THIS
    float gameViewHeight = 720.0f;   // ADD THIS
};
```

## Step 2: Update Scene Serializer

**File: `src/editor/SceneSerializer.cpp`**

In `saveBinary()`:
```cpp
// After writing grid settings, add:
writeFloat(out, scene.gameViewWidth);
writeFloat(out, scene.gameViewHeight);
```

In `loadBinary()`:
```cpp
// After reading grid settings, add:
if (version >= 2) {
    scene.gameViewWidth = readFloat(in);
    scene.gameViewHeight = readFloat(in);
}
// For version 1, Scene struct defaults will be used
```

Update version number to 2 in `saveBinary()`.

Also update `saveJSON()` and `loadJSON()` to include game view dimensions.

## Step 3: Update Editor to Save/Load Game View

**File: `src/editor/Editor_Scene.cpp`**

In `saveScene()`:
```cpp
// Copy game view dimensions into scene before saving
currentScene.gameViewWidth = gameViewWidth;
currentScene.gameViewHeight = gameViewHeight;
```

In `loadScene()`:
```cpp
// Restore game view dimensions from loaded scene
gameViewWidth = currentScene.gameViewWidth;
gameViewHeight = currentScene.gameViewHeight;
```

Also update `loadScene()` to support both `.map` and `.json` files.

## Step 4: Add Play Button to Editor

**File: `src/editor/Editor_ImGui.cpp`**

In `renderImGuiPanel()`, add before the separator:
```cpp
// Play button
if (ImGui::Button("Play", ImVec2(-1, 0))) {
    // Auto-save if no path exists
    if (currentScene.path.empty()) {
        std::string defaultPath = "src/maps/" + currentScene.name;
        if (defaultPath.empty() || currentScene.name == "Untitled") {
            defaultPath = "src/maps/untitled";
        }
        currentScene.path = defaultPath + ".map";
    }
    
    // Save current game view dimensions
    currentScene.gameViewWidth = gameViewWidth;
    currentScene.gameViewHeight = gameViewHeight;
    
    // Remove .map extension if present (saveScene adds it)
    std::string savePath = currentScene.path;
    if (savePath.size() > 4 && savePath.substr(savePath.size() - 4) == ".map") {
        savePath = savePath.substr(0, savePath.size() - 4);
    }
    
    saveScene(savePath);
    
    // Store path for game to load
    playMapPath = currentScene.path;
    glfwSetWindowShouldClose(m_window.getHandle(), true);
}
```

**File: `src/editor/Editor.h`**

Add to public section:
```cpp
std::string getPlayMapPath() const { return playMapPath; }
```

Add to private section:
```cpp
std::string playMapPath = ""; // Set when Play button is clicked
```

## Step 5: Create Game Class

**File: `src/game/Game.h`**
- See the actual file for full implementation
- Key: Uses same rendering system as Editor (Shader, Camera, AssetManager)
- Loads map and renders entities without ImGui UI

**File: `src/game/Game.cpp`**
- Constructor: Loads map, sets up camera, loads textures
- `loadAssets()`: Only loads textures used by entities
- `drawEntities()`: Renders all entities
- `run()`: Main game loop

## Step 6: Update Main.cpp

**File: `src/main.cpp`**
```cpp
#include "./game/Game.h"
#include <GLFW/glfw3.h>

int main(int argc, char* argv[]) {
    Window window(1280, 720, "Tile2D");
    
    // Run Game if -game argument is passed
    if (argc > 1 && std::string(argv[1]) == "-game") {
        std::string mapPath = (argc > 2) ? argv[2] : "src/maps/saveMe.map.map";
        Game game(window, mapPath);
        game.run();
    } else {
        Editor editor(window);
        editor.run();
        
        // Check if Play button was clicked
        std::string playMapPath = editor.getPlayMapPath();
        if (!playMapPath.empty()) {
            // Reset window close flag
            glfwSetWindowShouldClose(window.getHandle(), false);
            Game game(window, playMapPath);
            game.run();
        }
    }
    
    return 0;
}
```

## Step 7: Add Game Files to Project

**File: `Tile2DEngine.vcxproj`**

Add to `<ClInclude>` section:
```xml
<ClInclude Include="src\game\Game.h" />
```

Add to `<ClCompile>` section:
```xml
<ClCompile Include="src\game\Game.cpp" />
```

## Step 8: Fix Header Guard

**File: `src/editor/Editor.h`**

Change:
```cpp
#ifndef GAME_HPP  // WRONG!
#define GAME_HPP
```

To:
```cpp
#ifndef EDITOR_H  // CORRECT
#define EDITOR_H
```

## Step 9: Fix Include Paths

**File: `src/editor/Editor.h`**

Change:
```cpp
#include "../Window.h"  // Wrong case
#include "../Camera.h"  // Wrong case
```

To:
```cpp
#include "../window.h"  // Correct case
#include "../camera.h"  // Correct case
```

## Testing Checklist

- [ ] Editor saves game view dimensions
- [ ] Editor loads game view dimensions
- [ ] Play button appears in Editor
- [ ] Play button saves scene and closes Editor
- [ ] Game launches after clicking Play
- [ ] Game loads map correctly
- [ ] Game uses correct camera dimensions
- [ ] Game renders entities with textures
- [ ] Command line `-game` argument works

## Common Issues

1. **Textures not showing**: Make sure scene has entities, Game only loads textures for entities
2. **Camera wrong size**: Check that game view dimensions are being saved/loaded
3. **Game closes immediately**: Window close flag needs to be reset before Game starts
4. **Compile errors**: Check header guards, include paths, and project file includes

