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
#include "./Editor_Imgui/GridModule.h"
#include "./Editor_Imgui/CameraModule.h"
#include "./Editor_Imgui/AssetModule.h"
#include "./Editor_Imgui/LayerModule.h"

class Editor {
public:

    GridModule grid{ cellWidth, cellHeight };
    CameraModule camera{ gameViewWidth, gameViewHeight };
    AssetModule assets{ assetList, selectedType };
    LayerModule layers{ placementLayer };

    explicit Editor(Window& window);
    
    ~Editor();
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

    static void staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void staticCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    void handleScroll(double xoffset, double yoffset);
    void handleMouseButton(int button, int action, int mods);
    void handleCursorPos(double xpos, double ypos);
    void setupCallbacks();
    void initImGui();
    void shutdownImGui();
    void renderImGuiPanel();
    void initGridBuffers();
    void processInput();
    void handleEntityPlacement();
    glm::vec2 getMouseWorldPosition();
    void drawInfiniteGrid();
    void drawEntities();    
    void newScene(const std::string& name);
    void saveScene(const std::string& path);
    void loadScene(const std::string& path);
    void openSceneDialog();
    void saveSceneDialog();
};

#endif
