#include "InputSystem.h"
#include "game/Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>

void InputSystem::Process(Scene& scene, float deltaTime, Window& winObj, Client& client) {
    GLFWwindow* window = winObj.window;
    auto view = scene.registry.view<TransformComponent, CameraComponent, InputComponent, RigidbodyComponent>();

    static double lastMouseX = winObj.width / 2.0;
    static double lastMouseY = winObj.height / 2.0;

    // Accumulators for input averaging
    static glm::vec3 accumulatedMoveDir(0.0f);
    static float accumulatedYaw = 0.0f;
    static float accumulatedPitch = 0.0f;
    static int sampleCount = 0;
    static bool jumpRequested = false;

    static float sendTimer = 0.0f;
    constexpr float sendInterval = 1.0f / 60.0f;

    view.each([&](auto entity, TransformComponent& transform, CameraComponent& camera, InputComponent& input, RigidbodyComponent& rb) {
        if (!input.enabled) return;

        input.lockMouse = true;
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
            input.lockMouse = false;
        }

        if (input.lockMouse) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);

            float deltaX = static_cast<float>(mouseX - lastMouseX);
            float deltaY = static_cast<float>(mouseY - lastMouseY);

            input.yaw -= deltaX * input.sensitivity;
            transform.rotation.y = input.yaw;

            input.pitch -= deltaY * input.sensitivity;
            input.pitch = glm::clamp(input.pitch, -89.0f, 89.0f);

            float pitchRad = glm::radians(input.pitch);
            float yawRad = glm::radians(input.yaw);

            glm::vec3 forward;
            forward.x = cos(pitchRad) * sin(yawRad);
            forward.y = sin(pitchRad);
            forward.z = cos(pitchRad) * cos(yawRad);

            camera.camera.Orientation = glm::normalize(forward);

            glfwSetCursorPos(window, winObj.width / 2.0, winObj.height / 2.0);
            lastMouseX = winObj.width / 2.0;
            lastMouseY = winObj.height / 2.0;

        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // WASD Movement
        input.moveDir = glm::vec3(0.0f);
        glm::vec3 flatForward = camera.camera.GetForward();
        flatForward.y = 0.0f;
        flatForward = glm::normalize(flatForward);

        glm::vec3 right = glm::normalize(glm::cross(flatForward, glm::vec3(0.0f, 1.0f, 0.0f)));

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            input.moveDir += flatForward;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            input.moveDir -= flatForward;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            input.moveDir += right;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            input.moveDir -= right;
        }

        // Jump flag
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && rb.isGrounded) {
            rb.wantsToJump = true;
            jumpRequested = true;
        }

        // Apply velocity locally (you might want to keep this for local prediction)
        if (rb.isGrounded) {
            if (glm::length(input.moveDir) > 0.01f) {
                glm::vec3 horizontalMove = glm::normalize(input.moveDir) * input.moveSpeed;
                rb.velocity.x = horizontalMove.x;
                rb.velocity.z = horizontalMove.z;
            } else {
                rb.velocity.x = 0.0f;
                rb.velocity.z = 0.0f;
            }
        }

        // Accumulate inputs for averaging
        accumulatedMoveDir += input.moveDir;
        accumulatedYaw += input.yaw;
        accumulatedPitch += input.pitch;
        sampleCount++;
    });

    // Update the timer for sending inputs
    sendTimer += deltaTime;
    if (sendTimer >= sendInterval && sampleCount > 0) {
        // Average the inputs
        glm::vec3 avgMoveDir = accumulatedMoveDir / (float)sampleCount;
        float avgYaw = accumulatedYaw / (float)sampleCount;
        float avgPitch = accumulatedPitch / (float)sampleCount;

        // Normalize avgMoveDir to avoid skew (if length is small, just zero)
        if (glm::length(avgMoveDir) > 0.01f) {
            avgMoveDir = glm::normalize(avgMoveDir);
        } else {
            avgMoveDir = glm::vec3(0.0f);
        }

        // Build a simple packet string or binary blob (example, adapt to your protocol)
        // Example: "INPUT yaw pitch moveX moveY moveZ jump"
        char packet[256];
        int len = snprintf(packet, sizeof(packet), "INPUT %.2f %.2f %.2f %.2f %.2f %d",
                           avgYaw, avgPitch,
                           avgMoveDir.x, avgMoveDir.y, avgMoveDir.z,
                           jumpRequested ? 1 : 0);

        // Send to server/client
        client.Send(std::string(packet, len), SEND_PLAYER_INPUT);

        // Reset accumulators and timer
        accumulatedMoveDir = glm::vec3(0.0f);
        accumulatedYaw = 0.0f;
        accumulatedPitch = 0.0f;
        sampleCount = 0;
        jumpRequested = false;
        sendTimer = 0.0f;
    }
}