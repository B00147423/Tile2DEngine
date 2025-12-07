#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>



class Window {
public:
    /**
     * Constructor: Creates a GLFW window and initializes OpenGL context.
     * Sets up OpenGL 3.3 context, initializes GLAD, sets viewport, and registers
     * framebuffer resize callback. Exits program on failure. This is the main window
     * that all rendering happens in.
     */
    Window(int width, int height, const std::string& title);
    
    /**
     * Destructor: Destroys the GLFW window and terminates GLFW.
     * Cleans up all window resources and shuts down GLFW. Called automatically
     * when Window goes out of scope.
     */
    ~Window();

    /**
     * Poll events: Processes all pending window events (input, resize, etc.).
     * Must be called every frame to keep the window responsive. This is what
     * makes input callbacks fire and updates the window state.
     */
    void pollEvents() const;
    
    /**
     * Swap buffers: Swaps the front and back buffers for double buffering.
     * Called at the end of each frame to display what was rendered. This is what
     * makes your drawing appear on screen. Without this, you'd see nothing.
     */
    void swapBuffers() const;
    
    /**
     * Should close: Returns true if the user requested to close the window.
     * Checked in the main loop to know when to exit. Returns true when user clicks
     * the X button or presses Alt+F4 (or whatever closes the window).
     */
    bool shouldClose() const;

    /**
     * Get handle: Returns the raw GLFW window pointer.
     * Used for setting callbacks, getting input, etc. This is needed because
     * GLFW functions require the window handle.
     */
    GLFWwindow* getHandle() const { return m_window; }
    
    /**
     * Get width: Returns the current window width in pixels.
     */
    int getWidth() const { return m_width; }
    
    /**
     * Get height: Returns the current window height in pixels.
     */
    int getHeight() const { return m_height; }

private:
    GLFWwindow* m_window = nullptr;
    int m_width;
    int m_height;
};


#endif