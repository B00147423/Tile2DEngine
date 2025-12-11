#pragma once
#include "EditorImguiModules.h"
#include "imgui.h"

struct LayerModule : public EditorImguiModules<LayerModule> {
    int& placementLayer;

    LayerModule(int& layer) : placementLayer(layer) {}

    void renderImpl() {
        static int selectedLayer = 0;
        ImGui::SliderInt("Layer", &selectedLayer, 0, 10);
        ImGui::Text("Placing on layer: %d", selectedLayer);
        placementLayer = selectedLayer;
        ImGui::Separator();
    }
};
