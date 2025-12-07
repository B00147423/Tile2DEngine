#include "Editor.h"
#include "AssetManager.h"

/**
 * Render ImGui panel: Draws the left-side editor panel UI.
 * Creates a fixed-size window on the left (kLeftPanelWidth wide) containing:
 * grid settings (cell width/height), camera view settings (game box size),
 * scene info, asset browser with clickable thumbnails, layer selector,
 * and save/load buttons. This is the main UI the user interacts with.
 */
void Editor::renderImGuiPanel() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(kLeftPanelWidth, windowHeight));
    ImGui::Begin("Editor", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    ImGui::Text("Topher Editor");
    ImGui::Separator();
    ImGui::Text("Grid Settings");
    ImGui::DragFloat("Cell Width", &cellWidth, 0.5f, 0.1f, 200.0f, "%.2f");
    ImGui::DragFloat("Cell Height", &cellHeight, 0.5f, 0.1f, 200.0f, "%.2f");

    ImGui::Text("Camera View (Game Box)");
    ImGui::DragFloat("Game Width", &gameViewWidth, 1.0f, 100.0f, 4000.0f, "%.0f");
    ImGui::DragFloat("Game Height", &gameViewHeight, 1.0f, 100.0f, 4000.0f, "%.0f");


    std::string title = "Scene: " + (currentScene.path.empty() ? "(Unsaved)" : currentScene.name);
    ImGui::Text("%s", title.c_str());
    ImGui::Separator();

    //  Create new blank scene instantly
    if (ImGui::Button("New Scene")) {
        newScene("Untitled");
        currentScene.path.clear();
        std::cout << "Created new scene: Untitled\n";
    }
    ImGui::Separator();
    ImGui::Text("Assets");
    ImGui::Separator();

    if (ImGui::BeginChild("AssetsScroll", ImVec2(0, 250), true)) {
        const int iconsPerRow = 4;
        int count = 0;

        for (const auto& asset : assetList) {
            std::string path = "src/assets/" + asset + ".png";
            GLuint texID = AssetManager::GetGPUHandle(path);

            if (texID == 0) {
                std::cerr << "Texture not found in AssetManager: " << path << std::endl;
                continue;
            }

            ImGui::PushID(asset.c_str());
            ImTextureRef texRef = { (ImTextureID)(intptr_t)texID };

            if (ImGui::ImageButton(
                ("##" + asset).c_str(),
                texRef,
                ImVec2(48, 48),
                ImVec2(0, 1), ImVec2(1, 0),
                ImVec4(0, 0, 0, 0),
                ImVec4(1, 1, 1, 1)))
            {
                selectedType = asset;
                std::cout << "Selected: " << asset << std::endl;
            }

            if (++count % iconsPerRow != 0)
                ImGui::SameLine();

            ImGui::PopID();
        }
        ImGui::EndChild();
    }

    // ⭐ NEW: Layer selection UI ⭐
    static int selectedLayer = 0;
    ImGui::SliderInt("Layer", &selectedLayer, 0, 10);
    ImGui::Text("Placing on layer: %d", selectedLayer);
    placementLayer = selectedLayer;
    ImGui::Separator();


    ImGui::Separator();
    openSceneDialog();
    saveSceneDialog();
    ImGui::End();

}
/**
 * Save scene dialog: Shows ImGui UI for saving scenes.
 * If currentScene.path is set, "Save" button saves immediately. Otherwise, shows
 * an input field for scene name and saves as "./src/maps/[name].map". Handles
 * both quick save and "Save As" scenarios. Updates currentScene.path after saving.
 */
void Editor::saveSceneDialog() {
    static bool showSaveAs = false;
    static char input[128] = "";

    if (ImGui::Button("Save")) {
        if (!currentScene.path.empty()) {
            saveScene(currentScene.path);
            std::cout << "Saved scene: " << currentScene.path << "\n";
        }
        else {
            showSaveAs = true;
        }
    }

    if (showSaveAs) {
        ImGui::Separator();
        ImGui::InputText("Scene name", input, IM_ARRAYSIZE(input));
        if (ImGui::Button("Save###SaveAsButton")) {
            /*std::string path = "./src/maps/" + std::string(input) + ".json";*/
            std::string path = "./src/maps/" + std::string(input) + ".map";
            saveScene(path);
            currentScene.path = path;
            showSaveAs = false;
            std::cout << "Saved new scene: " << currentScene.path << "\n";
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) showSaveAs = false;
    }
}

/**
 * Open scene dialog: Shows an ImGui modal window listing all available .map and .json files.
 * Scans the ./src/maps directory, displays each scene file as a clickable item.
 * When clicked, calls loadScene() with that file path and closes the dialog.
 * Triggered by "Open Scene" button or File menu.
 */
void Editor::openSceneDialog() {
    static bool openScene = false;
    static std::string folder = "./src/maps";

    // Trigger open scene window only when button or menu pressed
    if (ImGui::Button("Open Scene")) openScene = true;

    if (openScene) {
        ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Open Scene", &openScene)) {
            ImGui::Text("Available scenes:");
            ImGui::Separator();

            for (auto& entry : fs::directory_iterator(folder)) {
                  if (entry.path().extension() == ".map" || entry.path().extension() == ".json"){
                    std::string filename = entry.path().filename().string();
                    if (ImGui::Selectable(filename.c_str())) {
                        std::string path = folder + "/" + filename;
                        loadScene(path);
                        currentScene.path = path;
                        openScene = false; // close after loading
                    }
                }
            }

            ImGui::Separator();
            if (ImGui::Button("Close")) openScene = false;
            ImGui::End();
        }
    }
}
