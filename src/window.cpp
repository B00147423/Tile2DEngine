#include "window.h"
#include <iostream>


static void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

/**
 * Constructor: Creates a GLFW window and initializes OpenGL context.
 * Sets up OpenGL 3.3 context, initializes GLAD, sets viewport, and registers
 * framebuffer resize callback. Exits program on failure. This is the main window
 * that all rendering happens in.
 */
Window::Window(int width, int height, const std::string& title)
    : m_width(width), m_height(height)
{
    if (!glfwInit()) {

        std::cerr << "Failed to initialize GLFW\n";
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        std::exit(-1);
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        std::exit(-1);
    }

    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

/**
 * Destructor: Destroys the GLFW window and terminates GLFW.
 * Cleans up all window resources and shuts down GLFW. Called automatically
 * when Window goes out of scope.
 */
Window::~Window() {
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

/**
 * Poll events: Processes all pending window events (input, resize, etc.).
 * Must be called every frame to keep the window responsive. This is what
 * makes input callbacks fire and updates the window state.
 */
void Window::pollEvents() const {
    glfwPollEvents();
}

/**
 * Swap buffers: Swaps the front and back buffers for double buffering.
 * Called at the end of each frame to display what was rendered. This is what
 * makes your drawing appear on screen. Without this, you'd see nothing.
 */
void Window::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

/**
 * Should close: Returns true if the user requested to close the window.
 * Checked in the main loop to know when to exit. Returns true when user clicks
 * the X button or presses Alt+F4 (or whatever closes the window).
 */
bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}
