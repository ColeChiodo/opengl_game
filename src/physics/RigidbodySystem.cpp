#include "RigidbodySystem.h"

void RigidbodySystem::Process(Scene& scene, float deltaTime) {
    auto view = scene.registry.view<TransformComponent, RigidbodyComponent>();

    view.each([&](auto entity, TransformComponent& transform, RigidbodyComponent& rb) {
        if (rb.affectedByGravity) {
        
            if (rb.velocity.y < 0) {
                rb.acceleration.y += rb.gravity * rb.fallMultiplier;
            } else {
                rb.acceleration.y += rb.gravity;
            }
        
            if (rb.velocity.y == 0) {
                rb.isGrounded = true;
            } else {
                rb.isGrounded = false;
            }
        }

        rb.velocity += rb.acceleration * deltaTime;
        transform.translation += rb.velocity * deltaTime;

        rb.acceleration = {};
    });
}
