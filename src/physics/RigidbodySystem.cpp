#include "RigidbodySystem.h"
#include "BoxColliderSystem.h"

void RigidbodySystem::Process(Scene& scene, float deltaTime) {
    auto view = scene.registry.view<TransformComponent, RigidbodyComponent, BoxColliderComponent>();

    view.each([&](auto entity, TransformComponent& transform, RigidbodyComponent& rb, BoxColliderComponent& collider) {
        if (rb.affectedByGravity) {
            if (rb.velocity.y < 0)
                rb.acceleration.y += rb.gravity * rb.fallMultiplier;
            else
                rb.acceleration.y += rb.gravity;
        }

        rb.velocity += rb.acceleration * deltaTime;

        glm::vec3 movement = rb.velocity * deltaTime;
        glm::vec3 start = transform.translation;
        glm::vec3 end = start + movement;

        float closestHit = 1.0f;
        glm::vec3 finalMovement = movement;

        auto otherView = scene.registry.view<TransformComponent, BoxColliderComponent>();
        otherView.each([&](auto other, const TransformComponent& otherTransform, const BoxColliderComponent& otherCollider) {
            if (other == entity || otherCollider.isStatic == false) return;

            glm::vec3 otherCenter = otherTransform.translation + otherCollider.offset;
            glm::vec3 otherMin = otherCenter - otherCollider.size;
            glm::vec3 otherMax = otherCenter + otherCollider.size;

            float tHit;
            if (RayIntersectsAABB(start, glm::normalize(movement), otherMin, otherMax, tHit)) {
                float hitDist = tHit * glm::length(movement) - deltaTime;
                if (hitDist < glm::length(movement)) {
                    if (tHit < closestHit) {
                        closestHit = tHit;
                        finalMovement = glm::normalize(movement) * hitDist;

                        if (glm::abs(movement.x) > glm::abs(movement.z)) {
                            rb.velocity.x = 0.0f;
                        } else {
                            rb.velocity.z = 0.0f;
                        }
                    }
                }
            }
        });

        transform.translation += finalMovement;
        rb.acceleration = glm::vec3(0.0f);
    });
}