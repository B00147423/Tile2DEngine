
#include "./Editor/Editor.h"
#include "window.h"

int main() {
    Window window(1280, 720, "Topher Editor");
    Editor editor(window);
    editor.run();
    return 0;
}
