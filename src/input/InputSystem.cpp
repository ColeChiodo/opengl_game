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

    static float sendTimer = 0.0f;
    constexpr float sendInterval = 1.0f / 60.0f;

    view.each([&](auto entity, TransformComponent& transform, CameraComponent& camera, InputComponent& input, RigidbodyComponent& rb) {
        if (!input.enabled) return;
        
        static bool escPressedLastFrame = false;
        bool escPressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

        if (escPressed && !escPressedLastFrame) {
            input.lockMouse = !input.lockMouse;
        }

        escPressedLastFrame = escPressed;

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
        }

        // Local velocity for prediction
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

        sendTimer += deltaTime;
        if (sendTimer >= sendInterval) {
            glm::vec3 pos = transform.translation;
            float yaw = input.yaw;
            float pitch = input.pitch;

            // Format: "STATE posX posY posZ yaw pitch grounded jump"
            char packet[256];
            int len = snprintf(packet, sizeof(packet), "STATE %.2f %.2f %.2f %.2f %.2f %d %d",
                               pos.x, pos.y, pos.z,
                               yaw, pitch,
                               rb.isGrounded ? 1 : 0,
                               rb.wantsToJump ? 1 : 0);

            client.Send(std::string(packet, len), SEND_PLAYER_STATE);

            rb.wantsToJump = false; // reset jump flag
            sendTimer = 0.0f;
        }
    });
}
