#include "Editor.h"

/**
 * Handle entity placement: Processes mouse clicks to place/remove entities on the grid.
 * Gets mouse world position, snaps it to grid, then:
 * - LEFT CLICK: Places a new entity at the snapped position (if not duplicate on same layer)
 * - RIGHT CLICK: Removes any entity at the snapped position
 * Prevents placing duplicates at the same position on the same layer. Sets
 * entitiesNeedSorting=true when entities are added/removed. Called every frame.
 */
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
        Entity entity;
        entity.type = selectedType;
        entity.x = snappedX;
        entity.y = snappedY;
        entity.layer = placementLayer;

        // Prevent duplicates only on SAME LAYER
        bool alreadyPlaced = false;
        for (auto& existing : currentScene.entities) {
            if (existing.layer == entity.layer &&         // ⭐ only block SAME layer
                std::abs(existing.x - entity.x) < 0.1f &&
                std::abs(existing.y - entity.y) < 0.1f)
            {
                alreadyPlaced = true;
                break;
            }
        }

        if (!alreadyPlaced) {
            currentScene.entities.push_back(entity);
            entitiesNeedSorting = true; // Mark for sorting
            std::cout << "Placed entity: " << entity.type
                << " at (" << entity.x << ", " << entity.y << ")\n";
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
                entitiesNeedSorting = true; // Mark for sorting
                break;
            }
        }
    }
}

/**
 * Process input: Handles keyboard input for camera movement (WASD keys).
 * Moves the camera up/down/left/right based on key presses, with movement speed
 * proportional to zoom level. Updates cameraX/cameraY and syncs to the Camera object.
 * Currently not called in run() loop -  code for future use if needed.
 */
void Editor::processInput() {
    GLFWwindow* handle = m_window.getHandle();
    const float moveSpeed = 10.0f * zoom;

    if (glfwGetKey(handle, GLFW_KEY_W) == GLFW_PRESS) cameraY += moveSpeed;
    if (glfwGetKey(handle, GLFW_KEY_S) == GLFW_PRESS) cameraY -= moveSpeed;
    if (glfwGetKey(handle, GLFW_KEY_A) == GLFW_PRESS) cameraX -= moveSpeed;
    if (glfwGetKey(handle, GLFW_KEY_D) == GLFW_PRESS) cameraX += moveSpeed;

    m_camera.setPosition(cameraX, cameraY);
}

/**
 * Get mouse world position: Converts screen mouse coordinates to world/grid coordinates.
 * Takes the window mouse position, accounts for the left panel offset, flips Y axis
 * (OpenGL Y=0 is bottom), converts to viewport space, then applies camera zoom and
 * position to get the final world coordinates. Used for placing entities at the cursor.
 */
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
