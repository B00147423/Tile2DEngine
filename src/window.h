#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>



class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    void pollEvents() const;
    void swapBuffers() const;
    bool shouldClose() const;

    GLFWwindow* getHandle() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    GLFWwindow* m_window = nullptr;
    int m_width;
    int m_height;
};


#endif