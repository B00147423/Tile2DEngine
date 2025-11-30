# Tile2DEngine

A 2D tile-based level editor built with C++ and OpenGL, featuring an intuitive ImGui interface for scene creation and editing.

## Features

- Visual tile-based level editor
- Scene serialization (JSON format)
- Camera controls with zoom
- Grid-based entity placement
- Real-time rendering with OpenGL

## Requirements

### Software
- **Visual Studio 2022** (or later) with C++ desktop development workload
- **Windows 10/11** (x64)
- **vcpkg** - C++ package manager
- **Git** - For cloning vendor dependencies

### Hardware
- Graphics card with OpenGL 3.3+ support

## Setup Instructions

### 1. Install vcpkg

If you don't have vcpkg installed:

```powershell
# Clone vcpkg
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg

# Run the bootstrap script
cd C:\vcpkg
.\bootstrap-vcpkg.bat

# Integrate with Visual Studio (optional but recommended)
.\vcpkg integrate install
```

### 2. Install Required vcpkg Packages

Install the following packages using vcpkg:

```powershell
cd C:\vcpkg
.\vcpkg install glfw3:x64-windows
.\vcpkg install glad:x64-windows
.\vcpkg install glm:x64-windows
.\vcpkg install nlohmann-json:x64-windows
```

Or install all at once:

```powershell
.\vcpkg install glfw3 glad glm nlohmann-json:x64-windows
```

### 3. Configure vcpkg in Visual Studio

If you haven't integrated vcpkg globally, you need to set the vcpkg path in your project:

1. Open the project in Visual Studio
2. Right-click the project → **Properties**
3. Go to **Configuration Properties** → **VC++ Directories**
4. Add your vcpkg path to:
   - **Include Directories**: `C:\vcpkg\installed\x64-windows\include`
   - **Library Directories**: `C:\vcpkg\installed\x64-windows\lib`

Alternatively, set the `VCPKG_ROOT` environment variable to your vcpkg installation path.

### 4. Build the Project

1. Open `PixelBreaker.sln` in Visual Studio 2022
2. Select **x64** as the platform (required)
3. Select **Debug** or **Release** configuration
4. Build the solution (Ctrl+Shift+B or Build → Build Solution)

### 5. Run

The executable will be generated in:
- Debug: `x64\Debug\Tile2DEngine.exe`
- Release: `x64\Release\Tile2DEngine.exe`

**Note:** Make sure `glfw3.dll` is in the same directory as the executable (it should be copied automatically from vcpkg).

## Dependencies

### vcpkg Packages
- **glfw3** - Window and input management
- **glad** - OpenGL loader
- **glm** - Mathematics library for graphics
- **nlohmann-json** - JSON parsing and serialization

### Vendor Libraries (included in repository)
- **imgui** - Immediate mode GUI
- **stb_image.h** - Image loading (single header)

## Troubleshooting

### "Cannot open include file: 'glad/glad.h'"
- Make sure vcpkg packages are installed: `vcpkg list`
- Verify vcpkg integration: `vcpkg integrate install`
- Check that you're building for **x64** platform

### "LNK2019: unresolved external symbol"
- Ensure all vcpkg packages are installed for the correct triplet (`x64-windows`)
- Rebuild the solution after installing new packages

### "glfw3.dll not found"
- Copy `glfw3.dll` from `C:\vcpkg\installed\x64-windows\bin\` to your output directory
- Or add the vcpkg bin directory to your PATH


## Building for Other Platforms

This project is currently configured for Windows x64. To build for other platforms:

1. Install the appropriate vcpkg triplet (e.g., `x64-linux`, `x64-osx`)
2. Update include paths in the project file
3. Adjust library linking as needed

## License