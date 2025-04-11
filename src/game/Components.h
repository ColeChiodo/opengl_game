#pragma once

#include <string>

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "renderer/Model.h"
#include "renderer/Light.h"

// struct IDComponent {
//     UUID ID;

//     IDComponent() = default;
//     IDComponent(const IDComponent&) = default;
// };

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
        glm::mat4 rot = glm::toMat4(glm::quat(rotation));
        return glm::translate(glm::mat4(1.0f), translation) * rot * glm::scale(glm::mat4(1.0f), scale);
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

struct LightComponent {
    Light light;

    LightComponent() = default;
    LightComponent(Light light) : light(light) {}
    LightComponent(const LightComponent&) = default;
};

// TODO: 
// Primative Component (Plane, Cube, Sphere)
// Physics Components (RididBody, Cube Collider, Sphere Collider)
// Game Rule Components