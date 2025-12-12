#include "../Editor.h"
#include "../AssetManager.h"

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

    // Scene title
    std::string title = "Scene: " + (currentScene.path.empty() ? "(Unsaved)" : currentScene.name);
    ImGui::Text("%s", title.c_str());
    ImGui::Separator();

    // New Scene button
    if (ImGui::Button("New Scene")) {
        newScene("Untitled");
        currentScene.path.clear();
        std::cout << "Created new scene: Untitled\n";
    }
    ImGui::Separator();

    // Modular UI blocks
    grid.render();
    camera.render();
    assets.render();
    layers.render();

    // Save/Open dialogs
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
    if (ImGui::Button("Open Scene")) openScene = true;

    if (!openScene) return;

    ImGui::SetNextWindowSize(ImVec2(400, 450), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Open Scene", &openScene)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Available scenes:");
    ImGui::Separator();

    std::string folder = "./src/maps";

    // Scan folder dynamically every frame
    for (auto& entry : fs::directory_iterator(folder)) {
        if (entry.path().extension() != ".map" && entry.path().extension() != ".json")
            continue;

        std::string filename = entry.path().filename().string();
        ImGui::PushID(filename.c_str());

        ImGui::Text("%s", filename.c_str());
        ImGui::SameLine(250);

        if (ImGui::Button("Open")) {
            std::string path = folder + "/" + filename;
            loadScene(path);
            currentScene.path = path;
            openScene = false;
        }

        ImGui::SameLine(320);
        if (ImGui::Button("Delete")) {
            std::string path = folder + "/" + filename;
            try {
                fs::remove(path);
                if (currentScene.path == path) currentScene.path.clear();
                std::cout << "Deleted scene: " << path << "\n";
            }
            catch (const fs::filesystem_error& e) {
                std::cerr << "Failed to delete scene: " << e.what() << "\n";
            }
        }

        ImGui::PopID();
    }

    ImGui::Separator();
    if (ImGui::Button("Close")) openScene = false;

    ImGui::End();
}
