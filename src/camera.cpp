#include "camera.h"

/**
 * Constructor: Creates a camera with a virtual viewport size.
 * The virtual size defines the base coordinate system (like 1280x720 pixels).
 * The actual viewport size is set later with resize(). Used for 2D orthographic projection.
 */
Camera::Camera(float virtualWidth, float virtualHeight)
    : m_virtualWidth(virtualWidth), m_virtualHeight(virtualHeight),
    m_viewWidth((int)virtualWidth), m_viewHeight((int)virtualHeight) {
}

/**
 * Resize: Updates the actual viewport dimensions (window size).
 * Called when the window is resized. The aspect ratio affects the projection matrix.
 * This doesn't change the virtual size, just the actual rendering area.
 */
void Camera::resize(int width, int height) {
    m_viewWidth = width;
    m_viewHeight = height;
}

/**
 * Set position: Sets the camera's world position (center of the view).
 * The camera looks at this point in world space. Used for panning/scrolling.
 */
void Camera::setPosition(float x, float y) {
    m_position = { x, y };
}

/**
 * Move: Translates the camera by a delta amount.
 * Adds dx/dy to the current position. Convenience method for relative movement.
 */
void Camera::move(float dx, float dy) {
    m_position.x += dx;
    m_position.y += dy;
}

/**
 * Set zoom: Sets the zoom level (1.0 = normal, >1.0 = zoomed in, <1.0 = zoomed out).
 * Higher zoom values show less of the world (more detail). Affects the projection matrix.
 */
void Camera::setZoom(float zoom) {
    m_zoom = zoom;
}

/**
 * Set virtual size: Changes the base coordinate system size.
 * This is the "logical" size the camera uses for calculations, independent of window size.
 * Changing this affects how much world space is visible at zoom=1.0.
 */
void Camera::setVirtualSize(float width, float height) {
    m_virtualWidth = width;
    m_virtualHeight = height;
}

/**
 * Get position: Returns the camera's current world position (center point).
 */
glm::vec2 Camera::getPosition() const {
    return m_position;
}

/**
 * Get zoom: Returns the current zoom level.
 */
float Camera::getZoom() const {
    return m_zoom;
}

/**
 * Get projection: Computes and returns the orthographic projection matrix.
 * Calculates the view frustum based on position, zoom, virtual size, and aspect ratio.
 * This matrix transforms world coordinates to clip space. Called every frame for rendering.
 */
glm::mat4 Camera::getProjection() const {
    float aspect = static_cast<float>(m_viewWidth) / static_cast<float>(m_viewHeight);

    // divide by zoom, not multiply
    float halfHeight = (m_virtualHeight * 0.5f) / m_zoom;
    float halfWidth = halfHeight * aspect;

    return glm::ortho(
        m_position.x - halfWidth,
        m_position.x + halfWidth,
        m_position.y - halfHeight,
        m_position.y + halfHeight,
        -1.0f, 1.0f
    );
}

