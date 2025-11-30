#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float virtualWidth = 1280.0f, float virtualHeight = 720.0f);

    void resize(int width, int height);
    void setPosition(float x, float y);
    void move(float dx, float dy);
    void setZoom(float zoom);
    void setVirtualSize(float width, float height);
    void resize(float width, float height);
    glm::vec2 getPosition() const;
    float getZoom() const;
    glm::mat4 getProjection() const;

private:
    glm::vec2 m_position = { 0.0f, 0.0f };
    float m_zoom = 1.0f;

    float m_virtualWidth;
    float m_virtualHeight;

    int m_viewWidth;
    int m_viewHeight;
};
