#pragma once

#include <unordered_set>

#include "game/Scene.h"
#include "CollisionUtil.h"

class BoxColliderSystem {
    public:
        void Process(Scene& scene, float deltaTime);
    
    private:
        bool MasksOverlap(const std::unordered_set<int>& maskA, const std::unordered_set<int>& maskB);
        bool IsGrounded(entt::entity entity, Scene& scene, float rayLength, float slopeThreshold);
};