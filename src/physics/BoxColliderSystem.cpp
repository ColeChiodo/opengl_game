#include "BoxColliderSystem.h"

#include <glm/gtx/quaternion.hpp>
#include <cfloat>

void BoxColliderSystem::Process(Scene& scene, float deltaTime) {
    auto view = scene.registry.view<TransformComponent, BoxColliderComponent>();
    std::vector<std::tuple<entt::entity, TransformComponent*, BoxColliderComponent*>> colliders;

    view.each([&](auto entity, TransformComponent& transform, BoxColliderComponent& collider) {
        colliders.emplace_back(entity, &transform, &collider);
    });

    for (size_t i = 0; i < colliders.size(); ++i) {
        auto [entityA, transformA, colliderA] = colliders[i];

        for (size_t j = i + 1; j < colliders.size(); ++j) {
            auto [entityB, transformB, colliderB] = colliders[j];

            if (!MasksOverlap(colliderA->collisionMask, colliderB->collisionMask)) continue;

            OBB obbA = CreateOBB(*transformA, *colliderA);
            OBB obbB = CreateOBB(*transformB, *colliderB);

            CollisionManifold manifold = SAT3DTest(obbA, obbB);
            if (!manifold.collided) continue;

            RigidbodyComponent* rbA = scene.registry.try_get<RigidbodyComponent>(entityA);
            RigidbodyComponent* rbB = scene.registry.try_get<RigidbodyComponent>(entityB);

            bool isStaticA = colliderA->isStatic;
            bool isStaticB = colliderB->isStatic;

            glm::vec3 correction = manifold.normal * manifold.depth;

            if (!isStaticA && isStaticB) {
                transformA->translation -= correction;
            } else if (isStaticA && !isStaticB) {
                transformB->translation += correction;
            } else if (!isStaticA && !isStaticB) {
                transformA->translation -= correction * 0.5f;
                transformB->translation += correction * 0.5f;
            }
        }

        // Ground detection
        RigidbodyComponent* rb = scene.registry.try_get<RigidbodyComponent>(entityA);
        if (rb && !colliderA->isStatic) {
            rb->isGrounded = IsGrounded(entityA, scene, 0.05f, 45.0f);
        }
    }
}

void BoxColliderSystem::HitscanRaycast(Scene& scene, const int peerID, std::string input) {
    // Format: "FIRE originX originY originZ directionX directionY directionZ"
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }

    glm::vec3 origin = {
        std::stof(tokens[1]),
        std::stof(tokens[2]),
        std::stof(tokens[3])
    };

    glm::vec3 direction = {
        std::stof(tokens[4]),
        std::stof(tokens[5]),
        std::stof(tokens[6])
    };

    auto view = scene.registry.view<TransformComponent, BoxColliderComponent, WeaponComponent, NetworkedComponent>();

    view.each([&](auto entity, TransformComponent& transform, BoxColliderComponent& collider, WeaponComponent& weapon, NetworkedComponent& networked) {
        if (networked.peerID == peerID) return;

        OBB obb = CreateOBB(transform, collider);

        float tHit;
        if (RayIntersectsOBB(origin, direction, obb, tHit)) {
            std::cout << peerID << " Hit " << networked.peerID << std::endl;
        }
    });
}

bool BoxColliderSystem::MasksOverlap(const std::unordered_set<int>& maskA, const std::unordered_set<int>& maskB) {
    for (int a : maskA) {
        if (maskB.count(a)) return true;
    }
    return false;
}

bool BoxColliderSystem::IsGrounded(entt::entity entity, Scene& scene, float rayLength, float slopeThreshold) {
    const auto& transform = scene.registry.get<TransformComponent>(entity);
    const auto& collider = scene.registry.get<BoxColliderComponent>(entity);

    OBB obb = CreateOBB(transform, collider);

    glm::vec3 rayDir = glm::vec3(0.0f, -1.0f, 0.0f);
    bool grounded = false;

    std::vector<glm::vec3> bottomCorners;

    for (int x = -1; x <= 1; x += 2) {
        for (int z = -1; z <= 1; z += 2) {
            glm::vec3 corner =
                obb.center
                + (obb.orientation[0] * obb.halfSize.x * (float)x)  // +/- X (Right vector)
                - (obb.orientation[1] * obb.halfSize.y)             // -Y (Down vector)
                + (obb.orientation[2] * obb.halfSize.z * (float)z); // +/- Z (Forward vector)

            bottomCorners.push_back(corner);
        }
    }

    bool result = false;
    auto view = scene.registry.view<TransformComponent, BoxColliderComponent>();
    for (const glm::vec3& origin : bottomCorners) {
        view.each([&](auto other, TransformComponent& otherTransform, BoxColliderComponent& otherCollider) {
            if (other != entity) {
                OBB otherOBB = CreateOBB(otherTransform, otherCollider);
                float tHit;

                if (RayIntersectsOBB(origin, rayDir, otherOBB, tHit) && tHit <= rayLength) {
                    glm::vec3 surfaceNormal = glm::normalize(rayDir); // For future improvement: calculate actual normal
                    float angle = glm::dot(surfaceNormal, glm::vec3(0.0f, 1.0f, 0.0f));

                    if (angle <= cos(glm::radians(slopeThreshold))) {
                        result = true;
                    }
                }
            }
        });
    }

    return result;
}