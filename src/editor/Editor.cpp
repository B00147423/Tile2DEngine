//#include "Editor.h"
//
//// Define the global variables declared as extern in Editor.h
//Editor* s_currentGame = nullptr;
//bool s_isPanning = false;
//double s_lastX = 0.0, s_lastY = 0.0;
//float gameViewWidth = 2000.0f;
//float gameViewHeight = 720.0f;
//
//
//// ======== SHADERS ========
//static const char* vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec2 aPos;
//uniform mat4 uProjection;
//void main() {
//    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
//}
//)";
//
//static const char* fragmentShaderSource = R"(
//#version 330 core
//out vec4 FragColor;
//uniform vec3 uColor;
//void main() {
//    FragColor = vec4(uColor, 1.0);
//}
//)";
//
//static GLuint createShaderProgram() {
//    GLuint v = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(v, 1, &vertexShaderSource, nullptr);
//    glCompileShader(v);
//    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(f, 1, &fragmentShaderSource, nullptr);
//    glCompileShader(f);
//    GLuint p = glCreateProgram();
//    glAttachShader(p, v);
//    glAttachShader(p, f);
//    glLinkProgram(p);
//    glDeleteShader(v);
//    glDeleteShader(f);
//    return p;
//}
//
//Editor::Editor(Window& window) : m_window(window) {
//    s_currentGame = this;
//    m_shader = createShaderProgram();
//    initImGui();
//
//    m_camera.setPosition(cameraX, cameraY);
//    m_camera.setZoom(zoom);
//
//    auto* handle = m_window.getHandle();
//    newScene("Untitled");
//    namespace fs = std::filesystem;
//    std::string assetFolder = "src/assets";
//
//    if (!fs::exists(assetFolder)) {
//        std::cerr << "Assets folder not found: " << fs::absolute(assetFolder) << std::endl;
//    }
//    else {
//        for (auto& entry : fs::directory_iterator(assetFolder)) {
//            if (entry.is_regular_file() && entry.path().extension() == ".png") {
//                std::string name = entry.path().stem().string();
//                assetList.push_back(name);
//            }
//        }
//    }
//
//    if (!assetList.empty()) {
//        selectedType = assetList.front();
//    }
//    // === Sprite Shader Setup ===
//    const char* vs = R"(
//    #version 330 core
//    layout (location = 0) in vec2 aPos;
//    layout (location = 1) in vec2 aTex;
//    out vec2 TexCoord;
//    uniform mat4 uMVP;
//    void main() {
//        TexCoord = aTex;
//        gl_Position = uMVP * vec4(aPos, 0.0, 1.0);
//    })";
//
//    const char* fragSrc = R"(
//    #version 330 core
//    in vec2 TexCoord;
//    out vec4 FragColor;
//    uniform sampler2D uTexture;
//    void main() {
//        FragColor = texture(uTexture, TexCoord);
//    })";
//
//    auto createProgram = [&](const char* vsrc, const char* fsrc) {
//        GLuint vs = glCreateShader(GL_VERTEX_SHADER);
//        glShaderSource(vs, 1, &vsrc, nullptr);
//        glCompileShader(vs);
//        GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fs, 1, &fsrc, nullptr);
//        glCompileShader(fs);
//        GLuint program = glCreateProgram();
//        glAttachShader(program, vs);
//        glAttachShader(program, fs);
//        glLinkProgram(program);
//        glDeleteShader(vs);
//        glDeleteShader(fs);
//        return program;
//        };
//
//    m_spriteShader = createProgram(vs, fragSrc);
//
//    // === Quad geometry ===
//    float verts[] = {
//        // pos      // tex
//        -0.5f, -0.5f, 0.0f, 0.0f,
//         0.5f, -0.5f, 1.0f, 0.0f,
//         0.5f,  0.5f, 1.0f, 1.0f,
//        -0.5f,  0.5f, 0.0f, 1.0f
//    };
//    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
//
//    glGenVertexArrays(1, &m_quadVAO);
//    glGenBuffers(1, &m_quadVBO);
//    glGenBuffers(1, &m_EBO);
//
//    glBindVertexArray(m_quadVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//    // === End sprite setup ===
//
//    // Chain both ImGui and your own callbacks
//    glfwSetScrollCallback(handle, [](GLFWwindow* w, double x, double y) {
//        ImGui_ImplGlfw_ScrollCallback(w, x, y);
//        scroll_callback(w, x, y);
//        });
//
//    glfwSetMouseButtonCallback(handle, [](GLFWwindow* w, int b, int a, int m) {
//        ImGui_ImplGlfw_MouseButtonCallback(w, b, a, m);
//        mouse_button_callback(w, b, a, m);
//        });
//
//    glfwSetCursorPosCallback(handle, [](GLFWwindow* w, double x, double y) {
//        ImGui_ImplGlfw_CursorPosCallback(w, x, y);
//        cursor_pos_callback(w, x, y);
//        });
//}
//
//Editor::~Editor() {
//    shutdownImGui();
//    glDeleteProgram(m_shader);
//}
//
//void Editor::initImGui() {
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGui::StyleColorsDark();
//    ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true);
//    ImGui_ImplOpenGL3_Init("#version 330");
//}
//
//void Editor::shutdownImGui() {
//    ImGui_ImplOpenGL3_Shutdown();
//    ImGui_ImplGlfw_Shutdown();
//    ImGui::DestroyContext();
//}
//
//void Editor::run() {
//    while (!m_window.shouldClose()) {
//        m_window.pollEvents();
//        glfwGetWindowSize(m_window.getHandle(), &windowWidth, &windowHeight);
//
//        ImGui_ImplOpenGL3_NewFrame();
//        ImGui_ImplGlfw_NewFrame();
//        ImGui::NewFrame();
//
//        // ===== MENU BAR =====
//        if (ImGui::BeginMainMenuBar()) {
//            if (ImGui::BeginMenu("File")) {
//                if (ImGui::MenuItem("New Scene")) newScene("Untitled");
//                if (ImGui::MenuItem("Open Scene...")) openSceneDialog();
//                if (ImGui::MenuItem("Save Scene")) saveScene(currentScene.path);
//                if (ImGui::MenuItem("Save Scene As...")) saveSceneDialog();
//                ImGui::Separator();
//                if (ImGui::MenuItem("Exit"))
//                    glfwSetWindowShouldClose(m_window.getHandle(), true);
//                ImGui::EndMenu();
//            }
//            ImGui::EndMainMenuBar();
//        }
//
//        renderImGuiPanel();
//
//        // ✨ Just call the new function
//        handleEntityPlacement();
//
//        // =========================
//        // DRAW SCENE (grid + sprites)
//        // =========================
//        glEnable(GL_SCISSOR_TEST);
//        glViewport(kLeftPanelWidth, 0, windowWidth - kLeftPanelWidth, windowHeight);
//        glScissor(kLeftPanelWidth, 0, windowWidth - kLeftPanelWidth, windowHeight);
//        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//        drawInfiniteGrid();
//        drawEntities();
//
//        glDisable(GL_SCISSOR_TEST);
//        glViewport(0, 0, windowWidth, windowHeight);
//
//        ImGui::Render();
//        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//        m_window.swapBuffers();
//    }
//}

#include "Editor.h"

// -------- Global Editor State --------
Editor* s_currentGame = nullptr;
bool s_isPanning = false;
double s_lastX = 0.0, s_lastY = 0.0;
float gameViewWidth = 2000.0f;
float gameViewHeight = 720.0f;


// =========================================================
// Constructor
// =========================================================
Editor::Editor(Window& window)
    : m_window(window)
{
    s_currentGame = this;

    // ===== Initialize ImGui =====
    initImGui();

    // ===== Camera setup =====
    m_camera.setPosition(cameraX, cameraY);
    m_camera.setZoom(zoom);

    // ===== Load shaders =====
    m_gridShader = Shader("src/shaders/grid.vert", "src/shaders/grid.frag");
    m_spriteShader = Shader("src/shaders/sprite.vert", "src/shaders/sprite.frag");

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


    // ===== Load asset list =====
    newScene("Untitled");

    std::string assetFolder = "src/assets";

    if (!fs::exists(assetFolder)) {
        std::cerr << "Assets folder not found: " << fs::absolute(assetFolder) << std::endl;
    } else {
        for (auto& entry : fs::directory_iterator(assetFolder)) {
            if (entry.is_regular_file() && entry.path().extension() == ".png") {
                assetList.push_back(entry.path().stem().string());
            }
        }
    }

    if (!assetList.empty())
        selectedType = assetList.front();

    // ===== Input callbacks =====
    auto* handle = m_window.getHandle();

    glfwSetScrollCallback(handle, [](GLFWwindow* w, double x, double y) {
        ImGui_ImplGlfw_ScrollCallback(w, x, y);
        scroll_callback(w, x, y);
    });

    glfwSetMouseButtonCallback(handle, [](GLFWwindow* w, int b, int a, int m) {
        ImGui_ImplGlfw_MouseButtonCallback(w, b, a, m);
        mouse_button_callback(w, b, a, m);
    });

    glfwSetCursorPosCallback(handle, [](GLFWwindow* w, double x, double y) {
        ImGui_ImplGlfw_CursorPosCallback(w, x, y);
        cursor_pos_callback(w, x, y);
    });
}


// =========================================================
// Destructor
// =========================================================
Editor::~Editor()
{
    shutdownImGui();
    // Shaders auto-clean through Shader destructor
        // Delete physics world
}


// =========================================================
// ImGui Init / Shutdown
// =========================================================
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


// =========================================================
// Run Loop
// =========================================================
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


