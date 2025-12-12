# Quick Reference - Game Mode

## File Structure
```
src/
├── main.cpp              ← Chooses Editor or Game mode
├── editor/
│   ├── Editor.h/cpp      ← Editor (creates maps)
│   ├── Scene.h           ← Added gameViewWidth/Height
│   ├── SceneSerializer   ← Saves/loads game view dims
│   └── Editor_ImGui.cpp  ← Added Play button
└── game/
    ├── Game.h            ← NEW: Game class
    └── Game.cpp          ← NEW: Loads maps, renders
```

## Key Changes Summary

### Scene Structure
- Added `gameViewWidth` and `gameViewHeight` to Scene
- These are saved/loaded with maps

### Editor
- Play button saves scene and launches Game
- Saves game view dimensions when saving
- Loads game view dimensions when loading

### Game
- Loads maps (binary or JSON)
- Uses camera dimensions from loaded scene
- Only loads textures for entities in scene
- Renders without editor UI

### Main
- Supports `-game` command line argument
- Launches Game after Editor if Play clicked

## Usage

**Editor Mode (default):**
```
Tile2DEngine.exe
```

**Game Mode (command line):**
```
Tile2DEngine.exe -game "src/maps/mylevel.map"
```

**Play from Editor:**
1. Load or create map
2. Click "Play" button
3. Game launches automatically

## Important Notes

- Game and Editor share rendering code (shaders, AssetManager)
- Game doesn't use ImGui (no editor UI)
- Game only loads textures for entities in the scene
- Camera dimensions are saved per-map in the Editor

