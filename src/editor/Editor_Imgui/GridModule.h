#pragma once
// GridModule.h
#include "EditorImguiModules.h"

struct GridModule : public EditorImguiModules<GridModule> {
    float& cellWidth;
    float& cellHeight;

    GridModule(float& w, float& h) : cellWidth(w), cellHeight(h) {}

    void renderImpl() {
        ImGui::Text("Grid Settings");
        ImGui::DragFloat("Cell Width", &cellWidth, 0.5f, 0.1f, 200.0f, "%.2f");
        ImGui::DragFloat("Cell Height", &cellHeight, 0.5f, 0.1f, 200.0f, "%.2f");
    }
};
