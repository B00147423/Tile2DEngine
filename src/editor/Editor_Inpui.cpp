#include "Editor.h"

static Editor* s_currentEditor = nullptr;
static bool s_isPanning = false;
static double s_lastX = 0.0, s_lastY = 0.0;

// callbacks here (scroll, mouse_button, cursor_pos)
void Editor::scroll_callback(GLFWwindow*, double, double yoffset) {
    if (!s_currentGame) return;

    const float zoomSpeed = 1.1f;
    if (yoffset > 0)
        s_currentGame->zoom /= zoomSpeed;
    else if (yoffset < 0)
        s_currentGame->zoom *= zoomSpeed;

    s_currentGame->zoom = glm::clamp(s_currentGame->zoom, 0.1f, 100.0f);
    s_currentGame->m_camera.setZoom(s_currentGame->zoom);
}

void Editor::mouse_button_callback(GLFWwindow* window, int button, int action, int) {
    if (!s_currentGame) return;

    if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
        if (action == GLFW_PRESS) {
            s_isPanning = true;
            glfwGetCursorPos(window, &s_lastX, &s_lastY);
        }
        else if (action == GLFW_RELEASE) {
            s_isPanning = false;
        }
    }
}

void Editor::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!s_currentGame || !s_isPanning) return;

    double dx = xpos - s_lastX;
    double dy = ypos - s_lastY;
    s_lastX = xpos;
    s_lastY = ypos;

    float zoom = s_currentGame->m_camera.getZoom();
    float moveFactor = (s_currentGame->cellWidth + s_currentGame->cellHeight) * 0.5f * 0.05f / zoom;

    s_currentGame->cameraX -= static_cast<float>(dx) * moveFactor;
    s_currentGame->cameraY += static_cast<float>(dy) * moveFactor;
    s_currentGame->m_camera.setPosition(s_currentGame->cameraX, s_currentGame->cameraY);
}

void Editor::handleEntityPlacement() {
    // Skip if mouse is over ImGui UI
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    glm::vec2 pos = getMouseWorldPosition();

    // Snap to grid
    float snappedX = std::floor(pos.x / cellWidth) * cellWidth + cellWidth * 0.5f;
    float snappedY = std::floor(pos.y / cellHeight) * cellHeight + cellHeight * 0.5f;

    // LEFT CLICK — place entity
    if (glfwGetMouseButton(m_window.getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        Entity e;
        e.type = selectedType;
        e.x = snappedX;
        e.y = snappedY;
        e.layer = placementLayer;   // ⭐ IMPORTANT ⭐

        // Prevent duplicates only on SAME LAYER
        bool alreadyPlaced = false;
        for (auto& existing : currentScene.entities) {
            if (existing.layer == e.layer &&         // ⭐ only block SAME layer
                std::abs(existing.x - e.x) < 0.1f &&
                std::abs(existing.y - e.y) < 0.1f)
            {
                alreadyPlaced = true;
                break;
            }
        }

        if (!alreadyPlaced) {
            currentScene.entities.push_back(e);
            std::cout << "Placed entity: " << e.type
                << " at (" << e.x << ", " << e.y << ")\n";
        }
    }

    //  RIGHT CLICK — remove entity
    if (glfwGetMouseButton(m_window.getHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        for (auto it = currentScene.entities.begin(); it != currentScene.entities.end(); ++it) {
            if (std::abs(it->x - snappedX) < cellWidth * 0.5f &&
                std::abs(it->y - snappedY) < cellHeight * 0.5f) {
                std::cout << "Removed entity: " << it->type
                    << " at (" << it->x << ", " << it->y << ")\n";
                currentScene.entities.erase(it);
                break;
            }
        }
    }
}

void Editor::processInput() {
    GLFWwindow* handle = m_window.getHandle();
    const float moveSpeed = 10.0f * zoom;

    if (glfwGetKey(handle, GLFW_KEY_W) == GLFW_PRESS) cameraY += moveSpeed;
    if (glfwGetKey(handle, GLFW_KEY_S) == GLFW_PRESS) cameraY -= moveSpeed;
    if (glfwGetKey(handle, GLFW_KEY_A) == GLFW_PRESS) cameraX -= moveSpeed;
    if (glfwGetKey(handle, GLFW_KEY_D) == GLFW_PRESS) cameraX += moveSpeed;

    m_camera.setPosition(cameraX, cameraY);
}

glm::vec2 Editor::getMouseWorldPosition() {
    double mouseX, mouseY;
    glfwGetCursorPos(m_window.getHandle(), &mouseX, &mouseY);

    // Convert from window to grid viewport coordinates
    float viewX = static_cast<float>(mouseX) - kLeftPanelWidth;
    float viewY = static_cast<float>(mouseY);
    viewY = windowHeight - viewY; // flip Y because OpenGL's Y=0 is bottom

    // Center of the grid viewport
    float halfW = (windowWidth - kLeftPanelWidth) * 0.5f;
    float halfH = windowHeight * 0.5f;

    // Apply camera zoom & position
    float worldX = (viewX - halfW) / m_camera.getZoom() + m_camera.getPosition().x;
    float worldY = (viewY - halfH) / m_camera.getZoom() + m_camera.getPosition().y;

    return { worldX, worldY };
}
