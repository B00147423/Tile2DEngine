#include "Editor.h"
#include "AssetManager.h"

/**
 * Draw infinite grid: Renders the background grid and camera view box.
 * Updates camera projection, calculates visible grid bounds based on camera position
 * and zoom, generates grid line vertices dynamically (only visible lines), uploads
 * them to the cached gridVBO, and draws as GL_LINES. Then draws a red box showing
 * the game view bounds (gameViewWidth x gameViewHeight). Uses cached VAOs/VBOs and
 * uniform locations for performance. Called every frame.
 */
void Editor::drawInfiniteGrid() {
    m_camera.resize(windowWidth - kLeftPanelWidth, windowHeight);
    glm::mat4 proj = m_camera.getProjection();

    // Use shader and cached uniform
    m_gridShader.use();
    glUniformMatrix4fv(uProjectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

    glm::vec2 camPos = m_camera.getPosition();
    float zoom = m_camera.getZoom();

    float halfW = (windowWidth - kLeftPanelWidth) * 0.5f / zoom;
    float halfH = windowHeight * 0.5f / zoom;

    float left = camPos.x - halfW * 2.0f;
    float right = camPos.x + halfW * 2.0f;
    float bottom = camPos.y - halfH * 2.0f;
    float top = camPos.y + halfH * 2.0f;

    float spacingX = cellWidth;
    float spacingY = cellHeight;

    // Use cached vector
    gridVertices.clear();
    gridVertices.reserve(20000);

    // Vertical lines
    float startX = std::floor(left / spacingX) * spacingX;
    for (float x = startX; x <= right; x += spacingX) {
        gridVertices.push_back(x); gridVertices.push_back(bottom);
        gridVertices.push_back(x); gridVertices.push_back(top);
    }

    // Horizontal lines
    float startY = std::floor(bottom / spacingY) * spacingY;
    for (float y = startY; y <= top; y += spacingY) {
        gridVertices.push_back(left);  gridVertices.push_back(y);
        gridVertices.push_back(right); gridVertices.push_back(y);
    }


    // Upload grid
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER,gridVertices.size() * sizeof(float), gridVertices.data(), GL_DYNAMIC_DRAW);

    glUniform3f(uGridColorLoc, 0.35f, 0.35f, 0.35f);
    glDrawArrays(GL_LINES, 0, gridVertices.size() / 2);

   
    // Draw red camera box
    boxVertices.clear();
    boxVertices.reserve(16);


    float halfViewW = gameViewWidth * 0.5f;
    float halfViewH = gameViewHeight * 0.5f;

    boxVertices = {
        -halfViewW, -halfViewH,  halfViewW, -halfViewH,
         halfViewW, -halfViewH,  halfViewW,  halfViewH,
         halfViewW,  halfViewH, -halfViewW,  halfViewH,
        -halfViewW,  halfViewH, -halfViewW, -halfViewH
    };

    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(float), boxVertices.data(), GL_STATIC_DRAW);

    glUniform3f(uGridColorLoc, 1.0f, 0.2f, 0.2f);
    glDrawArrays(GL_LINES, 0, boxVertices.size() / 2);
}

/**
 * Draw entities: Renders all entities in the scene as textured quads.
 * First sorts entities by layer if entitiesNeedSorting flag is set (optimization:
 * only sort when entities change). Then for each entity: gets its texture from
 * AssetManager, builds a model matrix (translation + scale), computes MVP matrix,
 * uploads it to shader, binds texture, and draws a quad. Entities are drawn in
 * layer order (lower layers first). Called every frame.
 */
void Editor::drawEntities() {
    glm::mat4 proj = m_camera.getProjection();

    // Sort only when needed
    if (entitiesNeedSorting) {
        std::sort(currentScene.entities.begin(), currentScene.entities.end(),
            [](const Entity& a, const Entity& b) { return a.layer < b.layer; });
        entitiesNeedSorting = false;
    }

    m_spriteShader.use();
    glBindVertexArray(m_quadVAO);

    GLuint lastTextureID = 0;   // Track last bound texture

    for (auto& e : currentScene.entities) {
        // Get cached path or create and cache it
        std::string& path = cachedTexturePaths[e.type];
        if (path.empty()) {
            path = "src/assets/" + e.type + ".png";
        }
        GLuint texID = AssetManager::GetGPUHandle(path);

        if (texID == 0) {
            std::cerr << "Missing texture: " << path << std::endl;
            continue;
        }

        // Only bind texture when it changes
        if (texID != lastTextureID) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texID);
            lastTextureID = texID;
        }

        float z = e.layer * 0.01f;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(e.x, e.y, z));
        model = glm::scale(model, glm::vec3(cellWidth, cellHeight, 1.0f));
        glm::mat4 mvp = proj * model;

        glUniformMatrix4fv(uMVPLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

