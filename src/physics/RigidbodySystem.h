#pragma once

#include "game/Scene.h"

class RigidbodySystem {
    public:
        void Process(Scene& scene, float deltaTime);
};