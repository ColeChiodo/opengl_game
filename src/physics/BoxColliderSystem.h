#pragma once

#include <unordered_set>

#include "game/Scene.h"
#include "CollisionUtil.h"

class BoxColliderSystem {
    public:
        void Process(Scene& scene, float deltaTime);
        void HitscanRaycast(Scene& scene, const int peerID, std::string input);
    
    private:
        bool MasksOverlap(const std::unordered_set<int>& maskA, const std::unordered_set<int>& maskB);
        bool IsGrounded(entt::entity entity, Scene& scene, float rayLength, float slopeThreshold);
};