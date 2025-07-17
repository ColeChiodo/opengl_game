#pragma once

class Scene;

class InterpolationSystem {
    public:
        void Process(Scene& scene, float deltaTime, int localPeerID);
};