#include "RigidbodySystem.h"
#include "BoxColliderSystem.h"

void RigidbodySystem::Process(Scene& scene, float deltaTime) {
    auto view = scene.registry.view<TransformComponent, RigidbodyComponent, BoxColliderComponent, InputComponent>();

    view.each([&](auto entity, TransformComponent& transform, RigidbodyComponent& rb, BoxColliderComponent& collider, InputComponent& input) {
        // Update timers
        if (rb.coyoteTimer > 0.0f)
            rb.coyoteTimer -= deltaTime;
        if (rb.jumpBufferTimer > 0.0f)
            rb.jumpBufferTimer -= deltaTime;

        // Jump buffering (set rb.jumpBufferTimer to jumpBufferTime on jump input)
        if (rb.wantsToJump) {
            rb.jumpBufferTimer = rb.jumpBufferTime;
            rb.wantsToJump = false;
        }

        // Gravity
        if (rb.affectedByGravity) {
            if (rb.velocity.y < 0)
                rb.acceleration.y += rb.gravity * rb.fallMultiplier;
            else
                rb.acceleration.y += rb.gravity;
        }

        // Apply jump
        if ((rb.isGrounded || rb.coyoteTimer > 0.0f) && rb.jumpBufferTimer > 0.0f) {
            rb.velocity.y = rb.jumpForce;

            // Optional bunnyhop boost
            float horizontalSpeed = glm::length(glm::vec2(rb.velocity.x, rb.velocity.z));
            if (horizontalSpeed > 0.0f) {
                float boostFactor = 1.05f;
                rb.velocity.x *= boostFactor;
                rb.velocity.z *= boostFactor;

                float newSpeed = glm::length(glm::vec2(rb.velocity.x, rb.velocity.z));
                if (newSpeed > rb.maxHorizontalSpeed) {
                    float scale = rb.maxHorizontalSpeed / newSpeed;
                    rb.velocity.x *= scale;
                    rb.velocity.z *= scale;
                }
            }

            rb.isGrounded = false;
            rb.coyoteTimer = 0.0f;
            rb.jumpBufferTimer = 0.0f;
        }

        // Strafe Air Control
        if (!rb.isGrounded && glm::length(input.moveDir) > 0.0f) {
            glm::vec3 wishDir = glm::normalize(glm::vec3(input.moveDir.x, 0.0f, input.moveDir.z));
            float wishSpeed = glm::length(glm::vec3(input.moveDir.x, 0.0f, input.moveDir.z)) * rb.maxHorizontalSpeed;

            glm::vec3 velocityHoriz(rb.velocity.x, 0.0f, rb.velocity.z);
            float currentSpeed = glm::dot(velocityHoriz, wishDir);
            float addSpeed = wishSpeed - currentSpeed;

            if (addSpeed > 0.0f) {
                float accelRate = rb.airControlStrength * deltaTime;
                float speedGain = accelRate * wishSpeed;

                if (speedGain > addSpeed)
                    speedGain = addSpeed;

                rb.velocity.x += wishDir.x * speedGain;
                rb.velocity.z += wishDir.z * speedGain;
            }

            // Clamp horizontal speed to max
            glm::vec2 horiz(rb.velocity.x, rb.velocity.z);
            float horizSpeed = glm::length(horiz);
            if (horizSpeed > rb.maxHorizontalSpeed) {
                glm::vec2 limited = glm::normalize(horiz) * rb.maxHorizontalSpeed;
                rb.velocity.x = limited.x;
                rb.velocity.z = limited.y;
            }
        }

        // Update velocity
        rb.velocity += rb.acceleration * deltaTime;

        glm::vec3 movement = rb.velocity * deltaTime;
        glm::vec3 start = transform.translation;
        glm::vec3 end = start + movement;

        float closestHit = 1.0f;
        glm::vec3 finalMovement = movement;

        auto otherView = scene.registry.view<TransformComponent, BoxColliderComponent>();
        otherView.each([&](auto other, const TransformComponent& otherTransform, const BoxColliderComponent& otherCollider) {
            if (other == entity || !otherCollider.isStatic) return;

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

                        // Lateral collision
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

        if (rb.isGrounded) {
            rb.coyoteTimer = rb.coyoteTime;
        }
    });
}