#include "RigidbodySystem.h"

void RigidbodySystem::Process(Scene& scene, float deltaTime) {
    auto view = scene.registry.view<TransformComponent, RigidbodyComponent>();

    view.each([&](auto entity, TransformComponent& transform, RigidbodyComponent& rb) {
        if (rb.affectedByGravity) {
            rb.acceleration.y = -9.8f;
        }

        rb.velocity += rb.acceleration * deltaTime;
        transform.translation += rb.velocity * deltaTime;

        rb.acceleration = {};
    });
}
