#pragma once

#include <string>
#include <sstream>
#include <unordered_set>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "core/Window.h"
#include "renderer/Model.h"
#include "renderer/Light.h"
#include "renderer/Primitive.h"
#include "renderer/Camera.h"

struct IDComponent {
    uint64_t ID;

    IDComponent() = default;
    IDComponent(const IDComponent&) = default;
};

struct NetworkedComponent {
    uint32_t peerID; // ENet peer's ID or a custom client ID

    NetworkedComponent() = default;
    NetworkedComponent(const NetworkedComponent&) = default;
};

struct InterpolationComponent {
    glm::vec3 previousPos;
    glm::vec3 previousRot;

    glm::vec3 targetPos;
    glm::vec3 targetRot;

    float elapsedTime = 0.0f;
    float duration = 0.1f;

    const int MS_PER_TICK = 1000 / 60;
};

struct TagComponent {
    std::string Tag;

    TagComponent() = default;
    TagComponent(const TagComponent&) = default;
    TagComponent(const std::string& tag) : Tag(tag) {}
};

struct TransformComponent {
    glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation) : translation(translation) {}

    glm::mat4 GetTransform() const {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, translation);
        mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        mat = glm::scale(mat, scale);
        return mat;
    }
};

struct ModelComponent {
    Model model;
    glm::vec3 modelPosition = glm::vec3(0.0f);
    glm::vec3 modelRotation = glm::vec3(0.0f);
    glm::vec3 modelScale = glm::vec3(1.0f);

    ModelComponent(const char* path) : model(path) {}

    glm::mat4 GetLocalTransform() const {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, modelPosition);
        mat = glm::rotate(mat, glm::radians(modelRotation.x), glm::vec3(1, 0, 0));
        mat = glm::rotate(mat, glm::radians(modelRotation.y), glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, glm::radians(modelRotation.z), glm::vec3(0, 0, 1));
        mat = glm::scale(mat, modelScale);
        return mat;
    }
};

struct PrimitiveComponent {
    Primitive primitive;
    glm::vec3 primitivePosition = glm::vec3(0.0f);
    glm::vec3 primitiveRotation = glm::vec3(0.0f);
    glm::vec3 primitiveScale = glm::vec3(1.0f);

    PrimitiveComponent() = default;
    PrimitiveComponent(Primitive primitive) : primitive(primitive) {}
    PrimitiveComponent(const PrimitiveComponent&) = default;

    glm::mat4 GetLocalTransform() const {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, primitivePosition);
        mat = glm::rotate(mat, glm::radians(primitiveRotation.x), glm::vec3(1, 0, 0));
        mat = glm::rotate(mat, glm::radians(primitiveRotation.y), glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, glm::radians(primitiveRotation.z), glm::vec3(0, 0, 1));
        mat = glm::scale(mat, primitiveScale);
        return mat;
    }
};

struct LightComponent {
    Light light;

    LightComponent() = default;
    LightComponent(Light light) : light(light) {}
    LightComponent(const LightComponent&) = default;
};

struct CameraComponent {
    Camera camera;
    bool isPrimary = false;
    glm::vec3 cameraPosition = glm::vec3(0.0f);
    glm::vec3 cameraRotation = glm::vec3(0.0f);
    glm::vec3 cameraScale = glm::vec3(1.0f);

    CameraComponent() = default;
    CameraComponent(Camera camera, Window& appWindow) : camera(camera) {
        glfwSetWindowUserPointer(appWindow.window, &camera);
    }
    CameraComponent(const CameraComponent&) = default;

    glm::mat4 GetLocalTransform() const {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, cameraPosition);
        mat = glm::rotate(mat, glm::radians(cameraRotation.x), glm::vec3(1, 0, 0));
        mat = glm::rotate(mat, glm::radians(cameraRotation.y), glm::vec3(0, 1, 0));
        mat = glm::rotate(mat, glm::radians(cameraRotation.z), glm::vec3(0, 0, 1));
        mat = glm::scale(mat, cameraScale);
        return mat;
    }
};

struct InputComponent {
    bool enabled = false;
    bool lockMouse = true;

    glm::vec3 moveDir = glm::vec3(0.0f);
    float pitch = 0.0f;
    float yaw = 0.0f;
    float moveSpeed = 8.0f;
    float sensitivity = 0.1f;

    bool wantsToJump = false;
    bool wantsToFire = false;
};

struct RigidbodyComponent {
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f);
    float mass = 1.0f;

    float gravity = -33.3f;
    float fallMultiplier = 1.5f;
    bool affectedByGravity = true;

    bool isGrounded = true;

    bool wantsToJump = false;
    float jumpForce = 10.0f;

    float maxHorizontalSpeed = 12.0f;
    float airControlStrength = 5.0f;

    float coyoteTime = 0.1f;
    float coyoteTimer = 0.0f;

    float jumpBufferTime = 0.1f;
    float jumpBufferTimer = 0.0f;
};

struct BoxColliderComponent {
    glm::vec3 size = glm::vec3(1.0f);
    glm::vec3 offset = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1, 0, 0, 0);
    bool isStatic = false;
    std::unordered_set<int> collisionMask = {0};
};


// TODO: 
// Game Rule Components