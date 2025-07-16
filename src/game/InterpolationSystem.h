#pragma once

#include "Scene.h"

class InterpolationSystem {
    public:
        void Process(Scene& scene, float deltaTime, int localPeerID);
};