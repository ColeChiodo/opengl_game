#pragma once

#include "game/Scene.h"
#include "core/Window.h"
#include <GLFW/glfw3.h>

class Input {
    public:
        void Process(Scene& scene, float deltaTime, Window& winObj);
};
