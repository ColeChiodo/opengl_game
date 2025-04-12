#include "Input.h"
#include "game/Entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>

void Input::Process(Scene& scene, float deltaTime, Window& winObj) {
    GLFWwindow* window = winObj.window;
    auto view = scene.registry.view<TransformComponent, CameraComponent, InputComponent>();

    static double lastMouseX = winObj.width / 2.0;
    static double lastMouseY = winObj.height / 2.0;

    view.each([&](auto entity, TransformComponent& transform, CameraComponent& camera, InputComponent& input) {
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

            // Update yaw from mouse X movement
            input.yaw -= deltaX * input.sensitivity;
            transform.rotation.y = input.yaw;

            // Update pitch from mouse Y movement (camera only)
            input.pitch -= deltaY * input.sensitivity;
            input.pitch = glm::clamp(input.pitch, -89.0f, 89.0f);

            // Calculate direction from pitch + yaw
            float pitchRad = glm::radians(input.pitch);
            float yawRad = glm::radians(input.yaw);

            glm::vec3 forward;
            forward.x = cos(pitchRad) * sin(yawRad);
            forward.y = sin(pitchRad);
            forward.z = cos(pitchRad) * cos(yawRad);

            camera.camera.Orientation = glm::normalize(forward);

            // Center mouse and update lastMouse values
            glfwSetCursorPos(window, winObj.width / 2.0, winObj.height / 2.0);
            lastMouseX = winObj.width / 2.0;
            lastMouseY = winObj.height / 2.0;

            // WASD Movement
            glm::vec3 moveDir(0.0f);
            glm::vec3 flatForward = camera.camera.GetForward();
            flatForward.y = 0.0f;
            flatForward = glm::normalize(flatForward);

            glm::vec3 right = glm::normalize(glm::cross(flatForward, glm::vec3(0.0f, 1.0f, 0.0f)));

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) moveDir += flatForward;
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) moveDir -= flatForward;
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) moveDir += right;
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) moveDir -= right;

            if (glm::length(moveDir) > 0.01f) {
                transform.translation += glm::normalize(moveDir) * input.moveSpeed * deltaTime;
            }
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    });
}
