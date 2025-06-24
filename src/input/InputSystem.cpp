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
        // TODO, edit the rigidbody by adding acceleration instead of directly changing the transform
        input.moveDir = glm::vec3(0.0f);
        glm::vec3 flatForward = camera.camera.GetForward();
        flatForward.y = 0.0f;
        flatForward = glm::normalize(flatForward);

        glm::vec3 right = glm::normalize(glm::cross(flatForward, glm::vec3(0.0f, 1.0f, 0.0f)));

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
        
        // dont send key strokes. limit the freq of packets to hz of server.

        // if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        //     input.moveDir += flatForward;
        //     client.Send("W");
        // }
        // if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        //     input.moveDir -= flatForward;
        //     client.Send("S");
        // }
        // if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        //     input.moveDir += right;
        //     client.Send("D");
        // }
        // if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        //     input.moveDir -= right;
        //     client.Send("A");
        // }

        // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && rb.isGrounded) {
        //     rb.wantsToJump = true;
        //     client.Send("Jump");
        // }
    });
}
