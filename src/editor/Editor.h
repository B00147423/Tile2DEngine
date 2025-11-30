//#pragma once
//#ifndef GAME_HPP
//#define GAME_HPP
//
//// Core dependencies
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <unordered_map>
//#include <filesystem>
//
//// OpenGL + GLM
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/common.hpp>
//
//// ImGui
//#include <imgui.h>
//#include <backends/imgui_impl_glfw.h>
//#include <backends/imgui_impl_opengl3.h>
//
//// JSON
//#include <nlohmann/json.hpp>
//using json = nlohmann::json;
//namespace fs = std::filesystem;
//
//// Engine / Game includes
//#include "../Window.h"
//#include "../Texture.h"
//#include "../Camera.h" 
//
//// =========================================================
//// Basic Data Types
//// =========================================================
//struct Entity {
//    std::string type = "";
//    float x = 0.0f;
//    float y = 0.0f;
//};
//    
//struct GridSettings {
//    float cellWidth, cellHeight;
//    int rows, cols;
//};
//
//struct Scene {
//    std::string name;
//    GridSettings grid;
//    std::vector<Entity> entities;
//    std::string path;
//};
//
//// =========================================================
//// Editor Class
//// =========================================================
//class Editor {
//public:
//    explicit Editor(Window& window);
//    ~Editor();
//
//    void run();
//
//    // Public variables (for ImGui + callbacks)
//    int windowWidth = 1280;
//    int windowHeight = 720;
//    float cameraX = 0.0f;
//    float cameraY = 0.0f;
//    float zoom = 1.0f;
//    float cellWidth = 16.0f;
//    float cellHeight = 16.0f;
//
//    static constexpr int kLeftPanelWidth = 320;
//
//    Camera m_camera;
//
//private:
//    void initImGui();
//    void shutdownImGui();
//    void renderImGuiPanel();
//    void processInput();
//    void drawInfiniteGrid();
//    void handleEntityPlacement();
//
//    // Scene management
//    void newScene(const std::string& name);
//    void saveScene(const std::string& path);
//    void loadScene(const std::string& path);
//    void openSceneDialog();
//    void saveSceneDialog();
//    void drawEntities();
//    static void scroll_callback(GLFWwindow*, double, double yoffset);
//
//    static void mouse_button_callback(GLFWwindow* window, int button, int action, int);
//
//    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
//
//    glm::vec2 getMouseWorldPosition();
//
//    Window& m_window;
//    unsigned int m_shader = 0;
//
//    // Scene state
//    Scene currentScene;
//    std::vector<std::string> assetList;
//    std::string selectedType = "";
//    GLuint m_spriteShader = 0;
//    GLuint m_quadVAO = 0, m_quadVBO = 0, m_EBO = 0;
//
//    std::unordered_map<std::string, Texture*> m_textureCache;
//};
//
//// =========================================================
//// Global editor state variables (declared here)
//// =========================================================
//extern Editor* s_currentGame;
//extern bool s_isPanning;
//extern double s_lastX, s_lastY;
//extern float gameViewWidth;
//extern float gameViewHeight;
//
//#endif // GAME_HPP


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
#include "../Texture.h"
#include "../Camera.h"

// Your data types (clean!)
#include "Entity.h"
#include "GridSettings.h"
#include "Scene.h"
#include "Shader.h"
#include "EditorState.h"

class Editor {
public:
    explicit Editor(Window& window);
    ~Editor();

    void run();

    // For ImGui + callbacks
    int windowWidth = 1280;
    int windowHeight = 720;
    float cameraX = 0.0f;
    float cameraY = 0.0f;
    float zoom = 1.0f;
    float cellWidth = 16.0f;
    float cellHeight = 16.0f;
    int placementLayer = 0;
    static constexpr int kLeftPanelWidth = 320;

    Camera m_camera;

private:
    void initImGui();
    void shutdownImGui();
    void renderImGuiPanel();
    void processInput();
    void drawInfiniteGrid();
    void handleEntityPlacement();

    // Scene management
    void newScene(const std::string& name);
    void saveScene(const std::string& path);
    void loadScene(const std::string& path);
    void openSceneDialog();
    void saveSceneDialog();
    void drawEntities();

    // Callbacks
    static void scroll_callback(GLFWwindow*, double, double yoffset);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int);
    static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);

    glm::vec2 getMouseWorldPosition();

private:
    Window& m_window;

    Shader m_gridShader;
    Shader m_spriteShader;

    GLuint m_quadVAO = 0, m_quadVBO = 0, m_EBO = 0;

    Scene currentScene;
    std::vector<std::string> assetList;
    std::string selectedType = "";

    std::unordered_map<std::string, Texture*> m_textureCache;
};

// Globals
extern Editor* s_currentGame;
extern bool s_isPanning;
extern double s_lastX, s_lastY;
extern float gameViewWidth;
extern float gameViewHeight;

#endif
