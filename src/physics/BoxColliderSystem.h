#pragma once

#include <unordered_set>

#include "game/Scene.h"

class BoxColliderSystem {
    public:
        void Process(Scene& scene, float deltaTime);
    
    private:
        bool AABBTest(const glm::vec3& aMin, const glm::vec3& aMax, const glm::vec3& bMin, const glm::vec3& bMax);
        bool MasksOverlap(const std::unordered_set<int>& maskA, const std::unordered_set<int>& maskB);
};