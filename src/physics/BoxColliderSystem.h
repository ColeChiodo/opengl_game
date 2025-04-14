#pragma once

#include <unordered_set>

#include "game/Scene.h"

// move to util file
bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& boxMin, const glm::vec3& boxMax, float& tHit);

class BoxColliderSystem {
    public:
        void Process(Scene& scene, float deltaTime);
    
    private:
        bool AABBTest(const glm::vec3& aMin, const glm::vec3& aMax, const glm::vec3& bMin, const glm::vec3& bMax);
        bool MasksOverlap(const std::unordered_set<int>& maskA, const std::unordered_set<int>& maskB);
        glm::vec3 CalculatePenetration(const glm::vec3& minA, const glm::vec3& maxA, const glm::vec3& minB, const glm::vec3& maxB);
        bool IsGrounded(entt::entity entity, Scene& scene, float rayLength);
};