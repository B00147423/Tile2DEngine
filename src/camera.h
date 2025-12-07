#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    /**
     * Constructor: Creates a camera with a virtual viewport size.
     * The virtual size defines the base coordinate system (like 1280x720 pixels).
     * The actual viewport size is set later with resize(). Used for 2D orthographic projection.
     */
    Camera(float virtualWidth = 1280.0f, float virtualHeight = 720.0f);

    /**
     * Resize: Updates the actual viewport dimensions (window size).
     * Called when the window is resized. The aspect ratio affects the projection matrix.
     * This doesn't change the virtual size, just the actual rendering area.
     */
    void resize(int width, int height);
    
    /**
     * Set position: Sets the camera's world position (center of the view).
     * The camera looks at this point in world space. Used for panning/scrolling.
     */
    void setPosition(float x, float y);
    
    /**
     * Move: Translates the camera by a delta amount.
     * Adds dx/dy to the current position. Convenience method for relative movement.
     */
    void move(float dx, float dy);
    
    /**
     * Set zoom: Sets the zoom level (1.0 = normal, >1.0 = zoomed in, <1.0 = zoomed out).
     * Higher zoom values show less of the world (more detail). Affects the projection matrix.
     */
    void setZoom(float zoom);
    
    /**
     * Set virtual size: Changes the base coordinate system size.
     * This is the "logical" size the camera uses for calculations, independent of window size.
     * Changing this affects how much world space is visible at zoom=1.0.
     */
    void setVirtualSize(float width, float height);

    /**
     * Get position: Returns the camera's current world position (center point).
     */
    glm::vec2 getPosition() const;
    
    /**
     * Get zoom: Returns the current zoom level.
     */
    float getZoom() const;
    
    /**
     * Get projection: Computes and returns the orthographic projection matrix.
     * Calculates the view frustum based on position, zoom, virtual size, and aspect ratio.
     * This matrix transforms world coordinates to clip space. Called every frame for rendering.
     */
    glm::mat4 getProjection() const;

private:
    glm::vec2 m_position = { 0.0f, 0.0f };
    float m_zoom = 1.0f;

    float m_virtualWidth;
    float m_virtualHeight;

    int m_viewWidth;
    int m_viewHeight;
};
