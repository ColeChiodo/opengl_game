#include "BoxColliderSystem.h"

void BoxColliderSystem::Process(Scene& scene, float deltaTime) {
    auto view = scene.registry.view<TransformComponent, BoxColliderComponent>();
    std::vector<std::tuple<entt::entity, glm::vec3, glm::vec3, bool, std::unordered_set<int>>> colliders;

    view.each([&](auto entity, TransformComponent& transform, BoxColliderComponent& collider) {
        // Create collider box
        glm::vec3 center = transform.translation + collider.offset;
        glm::vec3 min = center - collider.size;
        glm::vec3 max = center + collider.size;

        colliders.emplace_back(entity, min, max, collider.isStatic, collider.collisionMask);
    });

    for (size_t i = 0; i < colliders.size(); ++i) {
        auto [entityA, minA, maxA, isStaticA, maskA] = colliders[i];

        for (size_t j = i + 1; j < colliders.size(); ++j) {
            auto [entityB, minB, maxB, isStaticB, maskB] = colliders[j];

            if (!AABBTest(minA, maxA, minB, maxB)) continue;
            if (!MasksOverlap(maskA, maskB)) continue;

            // Get mutable components to resolve
            auto& transformA = scene.registry.get<TransformComponent>(entityA);
            auto& transformB = scene.registry.get<TransformComponent>(entityB);

            RigidbodyComponent* rbA = scene.registry.try_get<RigidbodyComponent>(entityA);
            RigidbodyComponent* rbB = scene.registry.try_get<RigidbodyComponent>(entityB);

            // Calculate penetration
            glm::vec3 penetration;
            penetration.x = std::min(maxA.x - minB.x, maxB.x - minA.x);
            penetration.y = std::min(maxA.y - minB.y, maxB.y - minA.y);
            penetration.z = std::min(maxA.z - minB.z, maxB.z - minA.z);

            float minAxis = std::min({penetration.x, penetration.y, penetration.z});

            glm::vec3 resolveDir = glm::vec3(0.0f);

            if (minAxis == penetration.x) {
                resolveDir.x = (transformA.translation.x < transformB.translation.x) ? -1.0f : 1.0f;
            } else if (minAxis == penetration.y) {
                resolveDir.y = (transformA.translation.y < transformB.translation.y) ? -1.0f : 1.0f;
            } else {
                resolveDir.z = (transformA.translation.z < transformB.translation.z) ? -1.0f : 1.0f;
            }

            glm::vec3 correction = resolveDir * minAxis;

            if (!isStaticA && isStaticB) {
                transformA.translation += correction;
                if (rbA) {
                    if (resolveDir.y != 0) rbA->velocity.y = 0.0f;
                    if (resolveDir.x != 0) rbA->velocity.x = 0.0f;
                    if (resolveDir.z != 0) rbA->velocity.z = 0.0f;
                }
            } else if (isStaticA && !isStaticB) {
                transformB.translation -= correction;
                if (rbB) {
                    if (resolveDir.y != 0) rbB->velocity.y = 0.0f;
                    if (resolveDir.x != 0) rbB->velocity.x = 0.0f;
                    if (resolveDir.z != 0) rbB->velocity.z = 0.0f;
                }
            } else if (!isStaticA && !isStaticB) {
                transformA.translation += correction * 0.5f;
                transformB.translation -= correction * 0.5f;
                if (rbA) rbA->velocity = glm::vec3(0.0f);
                if (rbB) rbB->velocity = glm::vec3(0.0f);
            }
        }
    }
}

bool BoxColliderSystem::AABBTest(const glm::vec3& aMin, const glm::vec3& aMax, const glm::vec3& bMin, const glm::vec3& bMax) {
    return (aMin.x <= bMax.x && aMax.x >= bMin.x) 
        && (aMin.y <= bMax.y && aMax.y >= bMin.y) 
        && (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

bool BoxColliderSystem::MasksOverlap(const std::unordered_set<int>& maskA, const std::unordered_set<int>& maskB) {
    for (int a : maskA) {
        if (maskB.count(a)) return true;
    }
    return false;
}
