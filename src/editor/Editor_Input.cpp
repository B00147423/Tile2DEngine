#include "Editor.h"

void Editor::handleEntityPlacement() {
    // Skip if mouse is over ImGui UI
    if (ImGui::GetIO().WantCaptureMouse)
        return;

    glm::vec2 pos = getMouseWorldPosition();

    // Snap to grid
    float snappedX = std::floor(pos.x / cellWidth) * cellWidth + cellWidth * 0.5f;
    float snappedY = std::floor(pos.y / cellHeight) * cellHeight + cellHeight * 0.5f;

    // --- Edge detection ---
    static bool leftWasPressed = false;
    static bool rightWasPressed = false;

    bool leftPressed = glfwGetMouseButton(m_window.getHandle(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool rightPressed = glfwGetMouseButton(m_window.getHandle(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    // LEFT CLICK — place entity
    if (leftPressed) {
        Entity entity;
        entity.type = selectedType;
        entity.x = snappedX;
        entity.y = snappedY;
        entity.layer = placementLayer;

        // Prevent duplicates only on SAME LAYER
        bool alreadyPlaced = false;
        for (auto& existing : currentScene.entities) {
            if (existing.layer == entity.layer &&   
                std::abs(existing.x - entity.x) < 0.1f &&
                std::abs(existing.y - entity.y) < 0.1f)
            {
                alreadyPlaced = true;
                break;
            }
        }

        if (!alreadyPlaced) {
            currentScene.entities.push_back(entity);
            entitiesNeedSorting = true;
            std::cout << "Placed entity: " << entity.type
                << " at (" << entity.x << ", " << entity.y << ")\n";
        }
    }

    //  RIGHT CLICK — remove entity
    if (rightPressed) {
        for (auto it = currentScene.entities.begin(); it != currentScene.entities.end(); ++it) {
            if (std::abs(it->x - snappedX) < cellWidth * 0.5f &&
                std::abs(it->y - snappedY) < cellHeight * 0.5f) {
                std::cout << "Removed entity: " << it->type
                    << " at (" << it->x << ", " << it->y << ")\n";
                currentScene.entities.erase(it);
                entitiesNeedSorting = true; // Mark for sorting
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
    // Get current window size (updated each frame in run loop)
    int currentWidth, currentHeight;
    glfwGetWindowSize(m_window.getHandle(), &currentWidth, &currentHeight);

    double mouseX, mouseY;
    glfwGetCursorPos(m_window.getHandle(), &mouseX, &mouseY);

    // Viewport dimensions (grid area, excluding left panel)
    int viewportWidth = currentWidth - kLeftPanelWidth;
    int viewportHeight = currentHeight;

    // Convert mouse position to viewport coordinates
    float viewX = static_cast<float>(mouseX) - kLeftPanelWidth;
    float viewY = static_cast<float>(mouseY);

    // Flip Y axis (OpenGL Y=0 is at bottom, screen Y=0 is at top)
    viewY = viewportHeight - viewY;

    // Normalize to [-1, 1] range in viewport space
    float normalizedX = (viewX / viewportWidth) * 2.0f - 1.0f;
    float normalizedY = (viewY / viewportHeight) * 2.0f - 1.0f;

    // Get camera properties
    glm::vec2 camPos = m_camera.getPosition();
    float zoom = m_camera.getZoom();

    // Match camera projection calculation exactly
    // Camera uses virtual size (default 1280x720) and aspect ratio
    float virtualWidth = 1280.0f;  // Match camera's m_virtualWidth
    float virtualHeight = 720.0f;   // Match camera's m_virtualHeight
    float aspect = static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight);

    float halfHeight = (virtualHeight * 0.5f) / zoom;
    float halfWidth = halfHeight * aspect;

    // Convert normalized coordinates to world space
    float worldX = camPos.x + normalizedX * halfWidth;
    float worldY = camPos.y + normalizedY * halfHeight;

    return { worldX, worldY };
}
