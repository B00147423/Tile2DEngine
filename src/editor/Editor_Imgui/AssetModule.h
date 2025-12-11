#pragma once

#include "EditorImguiModules.h"
#include <vector>
#include <string>

// AssetModule.h
#include "../AssetManager.h"

struct AssetModule : public EditorImguiModules<AssetModule> {
    std::vector<std::string>& assetList;
    std::string& selectedType;

    AssetModule(std::vector<std::string>& assets, std::string& selected)
        : assetList(assets), selectedType(selected) {
    }

    void renderImpl() {
        ImGui::Text("Assets");
        ImGui::Separator();
        if (ImGui::BeginChild("AssetsScroll", ImVec2(0, 250), true)) {
            const int iconsPerRow = 4;
            int count = 0;
            for (auto& asset : assetList) {
                std::string path = "src/assets/" + asset + ".png";
                GLuint texID = AssetManager::GetGPUHandle(path);
                if (texID == 0) continue;

                ImGui::PushID(asset.c_str());
                if (ImGui::ImageButton(
                    (std::string("##") + asset).c_str(),
                    (ImTextureID)(intptr_t)texID,
                    ImVec2(48, 48),
                    ImVec2(0, 1),  // top-left
                    ImVec2(1, 0)   // bottom-right
                )) {
                    selectedType = asset;
                }
                if (++count % iconsPerRow != 0) ImGui::SameLine();
                ImGui::PopID();
            }
            ImGui::EndChild();
        }
    }
};
