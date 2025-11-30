#include "camera.h"

Camera::Camera(float virtualWidth, float virtualHeight)
    : m_virtualWidth(virtualWidth), m_virtualHeight(virtualHeight),
    m_viewWidth((int)virtualWidth), m_viewHeight((int)virtualHeight) {
}

void Camera::resize(int width, int height) {
    m_viewWidth = width;
    m_viewHeight = height;
}

void Camera::setPosition(float x, float y) {
    m_position = { x, y };
}

void Camera::move(float dx, float dy) {
    m_position.x += dx;
    m_position.y += dy;
}

void Camera::setZoom(float zoom) {
    m_zoom = zoom;
}

void Camera::setVirtualSize(float width, float height) {
    m_virtualWidth = width;
    m_virtualHeight = height;
}

glm::vec2 Camera::getPosition() const {
    return m_position;
}

float Camera::getZoom() const {
    return m_zoom;
}

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

