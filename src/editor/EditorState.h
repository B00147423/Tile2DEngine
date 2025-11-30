#pragma once

struct EditorState {
    bool isPanning = false;
    double lastX = 0.0;
    double lastY = 0.0;
    float viewWidth = 2000.0f;
    float viewHeight = 720.0f;
};

extern EditorState GEditorState;
