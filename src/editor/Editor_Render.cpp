#include "Editor.h"
//
//void Editor::drawInfiniteGrid() {
//    m_camera.resize(windowWidth - kLeftPanelWidth, windowHeight);
//    glm::mat4 proj = m_camera.getProjection();
//
//    glUseProgram(m_shader);
//    glUniformMatrix4fv(glGetUniformLocation(m_shader, "uProjection"), 1, GL_FALSE, &proj[0][0]);
//
//    // === INFINITE GRID FIX ===
//    glm::vec2 camPos = m_camera.getPosition();
//    float zoom = m_camera.getZoom();
//
//    // How much world space fits in the screen
//    float halfW = (windowWidth - kLeftPanelWidth) * 0.5f / zoom;
//    float halfH = windowHeight * 0.5f / zoom;
//
//    // Draw enough lines to cover the screen area around the camera
//    float left = camPos.x - halfW * 2.0f;
//    float right = camPos.x + halfW * 2.0f;
//    float bottom = camPos.y - halfH * 2.0f;
//    float top = camPos.y + halfH * 2.0f;
//
//    // 🟢 DO NOT multiply spacing by zoom anymore
//    float spacingX = cellWidth;
//    float spacingY = cellHeight;
//
//    std::vector<float> vertices;
//
//    // Vertical lines
//    float startX = std::floor(left / spacingX) * spacingX;
//    for (float x = startX; x <= right; x += spacingX)
//        vertices.insert(vertices.end(), { x, bottom, x, top });
//
//    // Horizontal lines
//    float startY = std::floor(bottom / spacingY) * spacingY;
//    for (float y = startY; y <= top; y += spacingY)
//        vertices.insert(vertices.end(), { left, y, right, y });
//
//    GLuint VAO, VBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    glUniform3f(glGetUniformLocation(m_shader, "uColor"), 0.35f, 0.35f, 0.35f);
//    glDrawArrays(GL_LINES, 0, vertices.size() / 2);
//
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//
//    // === Draw red camera box ===
//    float halfViewW = (gameViewWidth * 0.5f);
//    float halfViewH = (gameViewHeight * 0.5f);
//    std::vector<float> box = {
//        -halfViewW, -halfViewH,  halfViewW, -halfViewH,
//         halfViewW, -halfViewH,  halfViewW,  halfViewH,
//         halfViewW,  halfViewH, -halfViewW,  halfViewH,
//        -halfViewW,  halfViewH, -halfViewW, -halfViewH
//    };
//
//    GLuint boxVAO, boxVBO;
//    glGenVertexArrays(1, &boxVAO);
//    glGenBuffers(1, &boxVBO);
//    glBindVertexArray(boxVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
//    glBufferData(GL_ARRAY_BUFFER, box.size() * sizeof(float), box.data(), GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    glUniform3f(glGetUniformLocation(m_shader, "uColor"), 1.0f, 0.2f, 0.2f);
//    glDrawArrays(GL_LINES, 0, box.size() / 2);
//
//    glDeleteVertexArrays(1, &boxVAO);
//    glDeleteBuffers(1, &boxVBO);
//}
//
//void Editor::drawEntities() {
//    glm::mat4 proj = m_camera.getProjection();
//    glUseProgram(m_spriteShader);
//    glBindVertexArray(m_quadVAO);
//
//    for (auto& e : currentScene.entities) {
//        Texture* tex = nullptr;
//
//        // Cache textures
//        if (m_textureCache.count(e.type))
//            tex = m_textureCache[e.type];
//        else {
//            std::string path = "src/assets/" + e.type + ".png"; // e.g. assets/enemy.png
//
//            tex = new Texture(path);
//            m_textureCache[e.type] = tex;
//        }
//
//        tex->bind();
//        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(e.x, e.y, 0.0f));
//        model = glm::scale(model, glm::vec3(cellWidth, cellHeight, 1.0f));
//        glm::mat4 mvp = proj * model;
//
//        glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
//        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//    }
//}


void Editor::drawInfiniteGrid() {
    m_camera.resize(windowWidth - kLeftPanelWidth, windowHeight);
    glm::mat4 proj = m_camera.getProjection();

    // ---- USE YOUR SHADER CLASS ----
    m_gridShader.use();
    glUniformMatrix4fv(
        glGetUniformLocation(m_gridShader.id, "uProjection"),
        1, GL_FALSE, glm::value_ptr(proj)
    );

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

    std::vector<float> vertices;

    float startX = std::floor(left / spacingX) * spacingX;
    for (float x = startX; x <= right; x += spacingX)
        vertices.insert(vertices.end(), { x, bottom, x, top });

    float startY = std::floor(bottom / spacingY) * spacingY;
    for (float y = startY; y <= top; y += spacingY)
        vertices.insert(vertices.end(), { left, y, right, y });

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ---- GRID COLOR ----
    glUniform3f(glGetUniformLocation(m_gridShader.id, "uColor"), 0.35f, 0.35f, 0.35f);
    glDrawArrays(GL_LINES, 0, vertices.size() / 2);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);


    // ------------ RED CAMERA BOX ------------
    float halfViewW = gameViewWidth * 0.5f;
    float halfViewH = gameViewHeight * 0.5f;

    std::vector<float> box = {
        -halfViewW, -halfViewH,  halfViewW, -halfViewH,
         halfViewW, -halfViewH,  halfViewW,  halfViewH,
         halfViewW,  halfViewH, -halfViewW,  halfViewH,
        -halfViewW,  halfViewH, -halfViewW, -halfViewH
    };

    GLuint boxVAO, boxVBO;
    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glBindVertexArray(boxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, box.size() * sizeof(float), box.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUniform3f(glGetUniformLocation(m_gridShader.id, "uColor"), 1.0f, 0.2f, 0.2f);
    glDrawArrays(GL_LINES, 0, box.size() / 2);

    glDeleteVertexArrays(1, &boxVAO);
    glDeleteBuffers(1, &boxVBO);
}


void Editor::drawEntities() {
    glm::mat4 proj = m_camera.getProjection();

    // Sort by layer (bottom to top)
    std::sort(currentScene.entities.begin(), currentScene.entities.end(),
        [](const Entity& a, const Entity& b) {
            return a.layer < b.layer;
        });

    m_spriteShader.use();

    glBindVertexArray(m_quadVAO);

    for (auto& e : currentScene.entities) {
        Texture* tex;

        if (m_textureCache.count(e.type))
            tex = m_textureCache[e.type];
        else {
            tex = new Texture("src/assets/" + e.type + ".png");
            m_textureCache[e.type] = tex;
        }

        tex->bind();

        float z = e.layer * 0.01f; //NEW layer depth

        glm::mat4 model = glm::translate(glm::mat4(1.0f),
            glm::vec3(e.x, e.y, z));

        model = glm::scale(model, glm::vec3(cellWidth, cellHeight, 1.0f));

        glm::mat4 mvp = proj * model;

        glUniformMatrix4fv(
            glGetUniformLocation(m_spriteShader.id, "uMVP"),
            1, GL_FALSE, glm::value_ptr(mvp)
        );

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}
