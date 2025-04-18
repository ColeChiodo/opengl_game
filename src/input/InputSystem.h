#pragma once

#include "game/Scene.h"
#include "core/Window.h"
#include <GLFW/glfw3.h>

class InputSystem {
    public:
        void Process(Scene& scene, float deltaTime, Window& winObj);
};
