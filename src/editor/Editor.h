#pragma once
#ifndef GAME_HPP
#define GAME_HPP

// Core
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

// OpenGL + GLM
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

// ImGui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace fs = std::filesystem;

// Engine Includes
#include "../Window.h"
#include "../Camera.h"

// Your data types (clean!)
#include "Entity.h"
#include "GridSettings.h"
#include "Scene.h"
#include "Shader.h"
#include "EditorState.h"
#include "./Editor_Imgui/GridModule.h"
#include "./Editor_Imgui/CameraModule.h"
#include "./Editor_Imgui/AssetModule.h"
#include "./Editor_Imgui/LayerModule.h"

class Editor {
    friend struct PlayModule;  // Allow PlayModule to access private members
public:

    GridModule grid{ cellWidth, cellHeight };
    CameraModule camera{ gameViewWidth, gameViewHeight };
    AssetModule assets{ assetList, selectedType };
    LayerModule layers{ placementLayer };
    /**
     * Constructor: Initializes the Editor with a reference to the Window.
     * Sets up GLFW user pointer for callbacks, initializes ImGui, loads shaders,
     * creates OpenGL buffers (VAOs/VBOs), loads all PNG assets from src/assets/,
     * and sets up input callbacks. This is where all the one-time setup happens.
     */
    explicit Editor(Window& window);
    
    /**
     * Destructor: Cleans up resources when the Editor is destroyed.
     * Shuts down ImGui, cleans up AssetManager, and OpenGL resources
     * (shaders auto-clean through their destructors).
     */
    ~Editor();

    /**
     * Main run loop: The heart of the editor. Runs continuously until the window closes.
     * Each frame it: polls events, updates ImGui, handles entity placement, renders
     * the grid and entities, renders ImGui UI, and swaps buffers. This is called
     * once from main() and runs until the user closes the window.
     */
    void run();
private:
    // Camera
    Camera m_camera;
    // Window and rendering
    Window& m_window;
    Shader m_gridShader;
    Shader m_spriteShader;


    //Accessed by ImGui via member functions
    int windowWidth = 1280;
    int windowHeight = 720;
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    float zoom = 1.0f;
    float cellWidth = 16.0f;
    float cellHeight = 16.0f;
    int placementLayer = 0;
    float gameViewWidth = 2000.0f;
    float gameViewHeight = 720.0f;
    static constexpr int kLeftPanelWidth = 320;

    

    std::vector<float> gridVertices;
    std::vector<float> boxVertices;
    GLuint m_quadVAO = 0, m_quadVBO = 0, m_EBO = 0;
    GLuint lastTextureID = 0;
    // Grid rendering
    GLuint gridVAO = 0;
    GLuint gridVBO = 0;
    GLuint boxVAO = 0;
    GLuint boxVBO = 0;

    // Cached uniform locations
    GLint uProjectionLoc = -1;
    GLint uGridColorLoc = -1;

    // Sprite shader MVP uniform
    GLint uMVPLoc = -1;

    // Flags
    bool gridBuffersInitialized = false;
    bool entitiesNeedSorting = true;
    // Scene state
    Scene currentScene;
    std::vector<std::string> assetList;
    std::string selectedType = "";
    
    // Cache texture paths per entity type to avoid string concatenation every frame
    std::unordered_map<std::string, std::string> cachedTexturePaths;

    // Input state
    bool isPanning = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    /**
     * Static scroll callback: GLFW requires static functions for callbacks, but we need
     * instance methods. This retrieves the Editor instance from GLFW's user pointer
     * and forwards the scroll event to handleScroll(). Also forwards to ImGui so it
     * can handle scrolling in UI elements. Called by GLFW when the mouse wheel is scrolled.
     */
    static void staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    /**
     * Static mouse button callback: GLFW callback for mouse button presses/releases.
     * Retrieves the Editor instance from the user pointer and forwards to handleMouseButton().
     * Also forwards to ImGui for UI interaction. Called by GLFW when any mouse button
     * is pressed or released (left, right, middle, etc.).
     */
    static void staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    
    /**
     * Static cursor position callback: GLFW callback for mouse movement.
     * Retrieves the Editor instance and forwards to handleCursorPos() for panning logic.
     * Also forwards to ImGui. Called continuously by GLFW as the mouse moves.
     */
    static void staticCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    /**
     * Handle scroll input: Processes mouse wheel scrolling to zoom in/out.
     * Multiplies or divides zoom by zoomSpeed (1.1) based on scroll direction,
     * clamps zoom between 0.1 and 100.0, and updates the camera. This is where
     * the actual zoom logic happens - the static callback just routes here.
     */
    void handleScroll(double xoffset, double yoffset);
    
    /**
     * Handle mouse button input: Processes mouse button presses, specifically
     * the middle mouse button for panning. When middle button is pressed, sets
     * isPanning=true and stores the current mouse position. When released, sets
     * isPanning=false. Left/right clicks are handled elsewhere (handleEntityPlacement).
     */
    void handleMouseButton(int button, int action, int mods);
    
    /**
     * Handle cursor movement: Processes mouse movement for camera panning.
     * Only does anything if isPanning is true (middle mouse held). Calculates
     * the delta movement, converts it to world space based on zoom level, and
     * updates cameraX/cameraY. This allows dragging the view around with middle mouse.
     */
    void handleCursorPos(double xpos, double ypos);

    /**
     * Setup callbacks: Registers all the static callback functions with GLFW.
     * This must be called after the window is created and the user pointer is set.
     * Connects GLFW's input events to our Editor instance methods via the static
     * forwarder functions. Called once in the constructor.
     */
    void setupCallbacks();



    /**
     * Initialize ImGui: Sets up the ImGui context, style, and platform/renderer bindings.
     * Creates the ImGui context, sets dark theme, and initializes GLFW and OpenGL3 backends.
     * Must be called before any ImGui calls. Called once in constructor.
     */
    void initImGui();
    
    /**
     * Shutdown ImGui: Cleans up ImGui resources when the editor closes.
     * Shuts down the OpenGL3 and GLFW backends, then destroys the ImGui context.
     * Called in destructor to prevent memory leaks.
     */
    void shutdownImGui();
    
    /**
     * Render ImGui panel: Draws the left-side editor panel UI.
     * Creates a fixed-size window on the left (kLeftPanelWidth wide) containing:
     * grid settings (cell width/height), camera view settings (game box size),
     * scene info, asset browser with clickable thumbnails, layer selector,
     * and save/load buttons. This is the main UI the user interacts with.
     */
    void renderImGuiPanel();

    /**
     * Initialize grid buffers: Creates and caches OpenGL VAOs/VBOs for grid rendering.
     * Creates gridVAO/gridVBO for drawing grid lines, and boxVAO/boxVBO for the camera
     * view box. Sets up vertex attribute pointers. Also caches uniform locations from
     * the shaders (uProjectionLoc, uGridColorLoc, uMVPLoc) to avoid lookups every frame.
     * MUST be called AFTER shaders are loaded (needs shader IDs). Called once in constructor.
     */
    void initGridBuffers();

    /**
     * Process input: Handles keyboard input for camera movement (WASD keys).
     * Moves the camera up/down/left/right based on key presses, with movement speed
     * proportional to zoom level. Updates cameraX/cameraY and syncs to the Camera object.
     * Currently not called in run() loop - may be legacy code or for future use.
     */
    void processInput();
    
    /**
     * Handle entity placement: Processes mouse clicks to place/remove entities on the grid.
     * Gets mouse world position, snaps it to grid, then:
     * - LEFT CLICK: Places a new entity at the snapped position (if not duplicate on same layer)
     * - RIGHT CLICK: Removes any entity at the snapped position
     * Prevents placing duplicates at the same position on the same layer. Sets
     * entitiesNeedSorting=true when entities are added/removed. Called every frame.
     */
    void handleEntityPlacement();
    
    /**
     * Get mouse world position: Converts screen mouse coordinates to world/grid coordinates.
     * Takes the window mouse position, accounts for the left panel offset, flips Y axis
     * (OpenGL Y=0 is bottom), converts to viewport space, then applies camera zoom and
     * position to get the final world coordinates. Used for placing entities at the cursor.
     */
    glm::vec2 getMouseWorldPosition();

    /**
     * Draw infinite grid: Renders the background grid and camera view box.
     * Updates camera projection, calculates visible grid bounds based on camera position
     * and zoom, generates grid line vertices dynamically (only visible lines), uploads
     * them to the cached gridVBO, and draws as GL_LINES. Then draws a red box showing
     * the game view bounds (gameViewWidth x gameViewHeight). Uses cached VAOs/VBOs and
     * uniform locations for performance. Called every frame.
     */
    void drawInfiniteGrid();
    
    /**
     * Draw entities: Renders all entities in the scene as textured quads.
     * First sorts entities by layer if entitiesNeedSorting flag is set (optimization:
     * only sort when entities change). Then for each entity: gets its texture from
     * AssetManager, builds a model matrix (translation + scale), computes MVP matrix,
     * uploads it to shader, binds texture, and draws a quad. Entities are drawn in
     * layer order (lower layers first). Called every frame.
     */
    void drawEntities();    

    /**
     * New scene: Creates a blank new scene with the given name.
     * Sets scene name, initializes grid settings from current cellWidth/cellHeight,
     * clears all entities, and sets entitiesNeedSorting=false (empty list doesn't need sorting).
     * Called when user clicks "New Scene" or from menu.
     */
    void newScene(const std::string& name);
    
    /**
     * Save scene: Saves the current scene to disk in both binary (.map) and JSON (.json) formats.
     * Binary format is fast for runtime loading, JSON is human-readable for debugging.
     * Uses SceneSerializer to do the actual file writing. Sets currentScene.path after
     * successful save. Prints errors if file operations fail. Called when user saves.
     */
    void saveScene(const std::string& path);
    
    /**
     * Load scene: Loads a scene from a binary .map file.
     * Uses SceneSerializer to read the binary file, populates currentScene with the
     * loaded data (name, grid settings, entities), sets the scene path, and marks
     * entities for sorting. Prints error if file doesn't exist or is invalid.
     * Called when user opens a scene from the dialog.
     */
    void loadScene(const std::string& path);
    
    /**
     * Open scene dialog: Shows an ImGui modal window listing all available .map and .json files.
     * Scans the ./src/maps directory, displays each scene file as a clickable item.
     * When clicked, calls loadScene() with that file path and closes the dialog.
     * Triggered by "Open Scene" button or File menu.
     */
    void openSceneDialog();
    
    /**
     * Save scene dialog: Shows ImGui UI for saving scenes.
     * If currentScene.path is set, "Save" button saves immediately. Otherwise, shows
     * an input field for scene name and saves as "./src/maps/[name].map". Handles
     * both quick save and "Save As" scenarios. Updates currentScene.path after saving.
     */
    void saveSceneDialog();
};

#endif
