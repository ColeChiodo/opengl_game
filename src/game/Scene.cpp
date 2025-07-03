#include "Scene.h"
#include "Entity.h"

Scene::Scene() {}

Scene::~Scene() {}

Entity Scene::CreateEntity(const std::string& name) {
    Entity entity = { registry.create(), this };
    entity.addComponent<TransformComponent>();
    auto& tag = entity.addComponent<TagComponent>();
    tag.Tag = name.empty() ? "Entity" : name;
    auto& id = entity.addComponent<IDComponent>();
    id.ID = totalEntities++;
    return entity;
}

void Scene::DestroyEntity(entt::entity entity) {
    registry.destroy(entity);
}

void Scene::HandlePlayerInput(const int peerID, const std::string& input) {
    // input in form: "INPUT yaw pitch moveX moveY moveZ jump"
    std::stringstream ss(input);
    std::string token;
    std::getline(ss, token, ' ');
    std::getline(ss, token, ' ');
    float yaw = std::stof(token);
    std::getline(ss, token, ' ');
    float pitch = std::stof(token);

    std::getline(ss, token, ' ');
    float moveX = std::stof(token);
    std::getline(ss, token, ' ');
    float moveY = std::stof(token);
    std::getline(ss, token, ' ');
    float moveZ = std::stof(token);
    glm::vec3 moveDir = glm::vec3(moveX, moveY, moveZ);

    std::getline(ss, token, ' ');
    bool jump = std::stoi(token);

    auto view = registry.view<NetworkedComponent, InputComponent, TransformComponent, CameraComponent, RigidbodyComponent>();

    for (auto entity : view) {
        auto& networked = view.get<NetworkedComponent>(entity);
        auto& input = view.get<InputComponent>(entity);

        if (networked.peerID == peerID && peerID != 0) {
            input.yaw = yaw;
            input.pitch = pitch;
            input.moveDir = moveDir;
            input.wantsToJump = jump;

            auto& transform = view.get<TransformComponent>(entity);
            auto& camera = view.get<CameraComponent>(entity);
            auto& rb = view.get<RigidbodyComponent>(entity);

            transform.rotation.y = input.yaw;
            glm::vec3 cameraRotation = glm::vec3(transform.rotation.x, transform.rotation.y, input.pitch);
            camera.camera.UpdateMatrix(90.0f, 0.1f, 1000.0f, transform.translation, cameraRotation);

            if (jump) {
                rb.wantsToJump = true;
            }

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

            break;
        }
    }
}

std::string Scene::Serialize() {
    std::string sceneData;
    
    // only add level name (tag of entity with id 0) and player entities
    auto view = registry.view<TagComponent, IDComponent>();

    for (auto entity : view) {
        auto& tag = view.get<TagComponent>(entity);
        auto& id = view.get<IDComponent>(entity);

        if (id.ID == 0) {
            sceneData += tag.Tag + "\n";
        }
    }

    for (auto entity : view) {
        auto& tag = view.get<TagComponent>(entity);
        auto& id = view.get<IDComponent>(entity);

        if (tag.Tag == "Player") {
            auto& peerID = registry.get<NetworkedComponent>(entity).peerID;
            sceneData += tag.Tag + " " + std::to_string(peerID) + "\n";
        }
    }

    return sceneData;
}
