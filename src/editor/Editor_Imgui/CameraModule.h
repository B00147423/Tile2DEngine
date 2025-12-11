#pragma once
// CameraModule.h
#include "EditorImguiModules.h"

struct CameraModule : public EditorImguiModules<CameraModule> {
    float& gameWidth;
    float& gameHeight;

    CameraModule(float& w, float& h) : gameWidth(w), gameHeight(h) {}

    void renderImpl() {
        ImGui::Text("Camera View");
        ImGui::DragFloat("Game Width", &gameWidth, 1.0f, 100.0f, 4000.0f, "%.0f");
        ImGui::DragFloat("Game Height", &gameHeight, 1.0f, 100.0f, 4000.0f, "%.0f");
    }
};
