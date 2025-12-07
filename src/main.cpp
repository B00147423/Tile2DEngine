
#include "./Editor/Editor.h"
#include "window.h"

int main() {
    Window window(1280, 720, "Tile2D");
    Editor editor(window);
    editor.run();
    return 0;
}
