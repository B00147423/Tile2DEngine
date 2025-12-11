# Tile2DEngine - Game Mode Implementation Summary

This document summarizes all changes made to add Game mode functionality to the Tile2DEngine.

## Overview

Added the ability to run the engine in two modes:
1. **Editor Mode** - Create and edit maps (existing functionality)
2. **Game Mode** - Play/run maps created in the editor (NEW)

## Files Created

### `src/game/Game.h`
- Game class header
- Handles loading maps and rendering them without editor UI
- Uses same rendering system as Editor (shaders, camera, AssetManager)

### `src/game/Game.cpp`
- Game class implementation
- Loads maps (supports both `.map` binary and `.json` formats)
- Renders entities using the same sprite rendering system
- Automatically centers camera on loaded entities
- Loads only textures needed by entities in the scene

## Files Modified

### `src/main.cpp`
**Changes:**
- Added command-line argument support: `-game "path/to/map.map"` to run Game mode
- Added logic to launch Game after Editor closes if Play button was clicked
- Resets window close flag before launching Game

**Usage:**
```cpp
// Default: Editor mode
Tile2DEngine.exe

// Game mode with specific map
Tile2DEngine.exe -game "src/maps/saveMe.map.map"

// Play button in Editor automatically launches Game
```

### `src/editor/Scene.h`
**Changes:**
- Added `gameViewWidth` and `gameViewHeight` fields to Scene struct
- These store the camera dimensions set in the Editor

### `src/editor/SceneSerializer.cpp`
**Changes:**
- Updated `saveBinary()` to save `gameViewWidth` and `gameViewHeight` (version 2)
- Updated `loadBinary()` to load game view dimensions (backward compatible with version 1)
- Updated `saveJSON()` to include game view dimensions
- Updated `loadJSON()` to load game view dimensions

**Binary Format (Version 2):**
```
[int] version (2)
[string] scene name
[float] grid.cellWidth
[float] grid.cellHeight
[int] grid.rows
[int] grid.cols
[float] gameViewWidth      <- NEW in version 2
[float] gameViewHeight     <- NEW in version 2
[int] entityCount
[entities...]
```

### `src/editor/Editor_Scene.cpp`
**Changes:**
- `saveScene()`: Copies `gameViewWidth`/`gameViewHeight` from Editor into Scene before saving
- `loadScene()`: Restores `gameViewWidth`/`gameViewHeight` from Scene to Editor after loading
- Added console output showing game view dimensions when saving/loading
- Updated to support loading both `.map` and `.json` files

### `src/editor/Editor_ImGui.cpp`
**Changes:**
- Added "Play" button in the editor panel
- Play button auto-saves scene if not saved yet
- Sets `playMapPath` when Play is clicked
- Closes editor window to launch Game

### `src/editor/Editor.h`
**Changes:**
- Fixed header guard from `GAME_HPP` to `EDITOR_H` (was conflicting with Game.h)
- Fixed include paths: `window.h` and `camera.h` (lowercase)
- Added `playMapPath` member variable
- Added public `getPlayMapPath()` method

### `Tile2DEngine.vcxproj`
**Changes:**
- Added `src\game\Game.h` to ClInclude section
- Added `src\game\Game.cpp` to ClCompile section

## How It Works

### Editor Mode (Default)
1. User creates/edits maps
2. Sets game view dimensions (Game Width/Height in UI)
3. Saves scene - game view dimensions are saved with the map
4. Clicks "Play" button
5. Editor saves scene and closes
6. Main.cpp detects Play was clicked and launches Game

### Game Mode
1. Game loads map file (binary or JSON)
2. Reads `gameViewWidth` and `gameViewHeight` from loaded scene
3. Sets camera virtual size to match those dimensions
4. Centers camera on entities automatically
5. Loads only textures needed by entities
6. Renders scene without editor UI

### Scene Loading Flow
```
Editor saves → SceneSerializer::saveBinary() → saves gameViewWidth/Height
Game loads → SceneSerializer::loadBinary() → reads gameViewWidth/Height
Game sets camera → m_camera.setVirtualSize(gameViewWidth, gameViewHeight)
```

## Key Features

1. **Game View Dimensions**: Editor saves camera dimensions with each map, Game uses them
2. **Auto Camera Centering**: Game automatically centers camera on loaded entities
3. **Texture Loading**: Game only loads textures used by entities (efficient)
4. **Dual Format Support**: Can load both `.map` (binary) and `.json` (text) files
5. **Seamless Transition**: Click Play in Editor → Game launches automatically

## Testing

1. **Load existing map:**
   - Open Editor
   - Click "Open Scene"
   - Select `saveMe.map.map`
   - Click "Play"
   - Game should load and show all entities with textures

2. **Create new map:**
   - Place entities in Editor
   - Set Game Width/Height
   - Click "Play" (auto-saves)
   - Game loads and displays

3. **Command line:**
   - `Tile2DEngine.exe -game "src/maps/saveMe.map.map"`

## Notes

- Game and Editor share the same rendering code (shaders, AssetManager, etc.)
- Game doesn't use ImGui (no editor UI)
- AssetManager is reset between Editor and Game (Editor calls Shutdown, Game calls Init)
- Game loads textures based on entity types in the scene, not all textures like Editor

