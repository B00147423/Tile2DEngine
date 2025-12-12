#include "Editor.h"
#include "SceneSerializer.h"
#include "AssetManager.h"

Editor::Editor(Window& window)
    : m_window(window)
{
    // Set this Editor instance as GLFW user pointer for callbacks
    glfwSetWindowUserPointer(m_window.getHandle(), this);
    
    AssetManager::Init();

    initImGui();
    m_camera.setPosition(cameraX, cameraY);
    m_camera.setZoom(zoom);
    m_camera.setVirtualSize(gameViewWidth, gameViewHeight); // Initialize camera virtual size

    // ===== Load shaders =====
    m_gridShader = Shader("src/shaders/grid.vert", "src/shaders/grid.frag");
    m_spriteShader = Shader("src/shaders/sprite.vert", "src/shaders/sprite.frag");
    
    // Initialize grid buffers AFTER shaders are loaded (needs shader IDs for uniform locations)
    initGridBuffers();

    // ===== Quad geometry =====
    float verts[] = {
        // pos      // tex
        -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 1.0f
    };

    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &m_quadVBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // Load asset list 
    newScene("Untitled");

    std::string assetFolder = "src/assets";

    if (!fs::exists(assetFolder)) {
        std::cerr << "Assets folder not found: " << fs::absolute(assetFolder) << std::endl;
    }
    else {
        std::vector<std::future<TextureData*>> futures;

        for (auto& entry : fs::directory_iterator(assetFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                std::string assetName = entry.path().stem().string();
                assetList.push_back(assetName);

                // Start async loading
                std::string fullPath = "src/assets/" + assetName + ".png";
                futures.push_back(AssetManager::LoadTextureAsync(fullPath));
            }
        }

        // ⭐ START TIMING ⭐
        auto startTime = std::chrono::high_resolution_clock::now();

        // Wait for all textures to load on CPU
        for (auto& future : futures) {
            future.wait();
        }

        auto cpuLoadTime = std::chrono::high_resolution_clock::now();
        std::cout << "CPU load time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(cpuLoadTime - startTime).count()
            << "ms\n";

        // Batch upload to GPU (FAST - single pass)
        AssetManager::UploadAllTexturesToGPU();

        auto gpuUploadTime = std::chrono::high_resolution_clock::now();
        std::cout << "GPU upload time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(gpuUploadTime - cpuLoadTime).count()
            << "ms\n";

        std::cout << "Total texture loading time: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(gpuUploadTime - startTime).count()
            << "ms\n";

        // Optional: Free CPU memory after upload
        AssetManager::FreeCPUDataForLoadedTextures();
    }

    if (!assetList.empty())
        selectedType = assetList.front();

    // ===== Setup input callbacks =====
    setupCallbacks();
}


Editor::~Editor()
{
    AssetManager::Shutdown();
    shutdownImGui();
    // Shaders auto-clean through Shader destructor
        // Delete physics world
}


void Editor::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Editor::shutdownImGui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::initGridBuffers() {
    if (gridBuffersInitialized) return;

    // Create VAO/VBO for grid
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    // Set up vertex attributes (position: 2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // Unbind

    // Create VAO/VBO for box
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    // Set up vertex attributes (position: 2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0); // Unbind

    // Cache uniform locations (must be called AFTER shaders are loaded!)
    uProjectionLoc = glGetUniformLocation(m_gridShader.id, "uProjection");
    uGridColorLoc = glGetUniformLocation(m_gridShader.id, "uColor");

    // Cache sprite MVP uniform
    uMVPLoc = glGetUniformLocation(m_spriteShader.id, "uMVP");

    // Verify uniform locations are valid
    if (uProjectionLoc == -1 || uGridColorLoc == -1 || uMVPLoc == -1) {
        std::cerr << "WARNING: Invalid uniform locations detected!\n";
        std::cerr << "  uProjectionLoc: " << uProjectionLoc << "\n";
        std::cerr << "  uGridColorLoc: " << uGridColorLoc << "\n";
        std::cerr << "  uMVPLoc: " << uMVPLoc << "\n";
    }

    gridBuffersInitialized = true;
}

void Editor::run()
{
    while (!m_window.shouldClose())
    {
        m_window.pollEvents();
        glfwGetWindowSize(m_window.getHandle(), &windowWidth, &windowHeight);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        // ===== MENU BAR =====
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene")) newScene("Untitled");
                if (ImGui::MenuItem("Open Scene...")) openSceneDialog();
                if (ImGui::MenuItem("Save Scene")) saveScene(currentScene.path);
                if (ImGui::MenuItem("Save Scene As...")) saveSceneDialog();
                ImGui::Separator();
                if (ImGui::MenuItem("Exit"))
                    glfwSetWindowShouldClose(m_window.getHandle(), true);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }


        renderImGuiPanel();
        handleEntityPlacement();


        // ================================
        // SCENE RENDERING
        // ================================
        glEnable(GL_SCISSOR_TEST);
        glViewport(kLeftPanelWidth, 0, windowWidth - kLeftPanelWidth, windowHeight);
        glScissor(kLeftPanelWidth, 0, windowWidth - kLeftPanelWidth, windowHeight);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        drawInfiniteGrid();
        drawEntities();

        glDisable(GL_SCISSOR_TEST);
        glViewport(0, 0, windowWidth, windowHeight);


        // ===== ImGui Render =====
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_window.swapBuffers();
    }
}

void Editor::staticScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    
    Editor* editor = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (editor) {
        editor->handleScroll(xoffset, yoffset);
    }
}

void Editor::staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    
    Editor* editor = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (editor) {
        editor->handleMouseButton(button, action, mods);
    }
}

void Editor::staticCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    
    Editor* editor = static_cast<Editor*>(glfwGetWindowUserPointer(window));
    if (editor) {
        editor->handleCursorPos(xpos, ypos);
    }
}

void Editor::handleScroll(double xoffset, double yoffset) {
    const float zoomSpeed = 1.1f;
    if (yoffset > 0)
        zoom /= zoomSpeed;
    else if (yoffset < 0)
        zoom *= zoomSpeed;

    zoom = std::max(0.1f, std::min(100.0f, zoom));
    m_camera.setZoom(zoom);
}

void Editor::handleMouseButton(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            isPanning = true;
            glfwGetCursorPos(m_window.getHandle(), &lastMouseX, &lastMouseY);
        }
        else if (action == GLFW_RELEASE) {
            isPanning = false;
        }
    }
}

void Editor::handleCursorPos(double xpos, double ypos) {
    if (!isPanning) return;

    double dx = xpos - lastMouseX;
    double dy = ypos - lastMouseY;
    lastMouseX = xpos;
    lastMouseY = ypos;

    float currentZoom = m_camera.getZoom();
    float moveFactor = (cellWidth + cellHeight) * 0.5f * 0.05f / currentZoom;

    cameraX -= static_cast<float>(dx) * moveFactor;
    cameraY += static_cast<float>(dy) * moveFactor;
    m_camera.setPosition(cameraX, cameraY);
}

void Editor::setupCallbacks() {
    auto* handle = m_window.getHandle();
    
    glfwSetScrollCallback(handle, staticScrollCallback);
    glfwSetMouseButtonCallback(handle, staticMouseButtonCallback);
    glfwSetCursorPosCallback(handle, staticCursorPosCallback);
}
