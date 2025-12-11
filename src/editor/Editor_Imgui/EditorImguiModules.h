#pragma once
#include "imgui.h"


template<typename Derived>

struct EditorImguiModules {
    void render() {
        static_cast<Derived*>(this)->renderImpl();
    }
};
