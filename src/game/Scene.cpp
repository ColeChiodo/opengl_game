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

void Scene::UpdatePlayerState(const int peerID, const std::string& input, const int localPeerID) {
    // Format: "STATE posX posY posZ velX velY velZ yaw pitch"
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }

    auto view = registry.view<NetworkedComponent, InputComponent, TransformComponent, CameraComponent, RigidbodyComponent, InterpolationComponent>();
    for (auto entity : view) {
        auto& networked = view.get<NetworkedComponent>(entity);
        if (networked.peerID == peerID) {
            glm::vec3 serverPos = {
                std::stof(tokens[1]),
                std::stof(tokens[2]),
                std::stof(tokens[3])
            };

            glm::vec3 serverVel = {
                std::stof(tokens[4]),
                std::stof(tokens[5]),
                std::stof(tokens[6])
            };

            float yaw = std::stof(tokens[7]);
            float pitch = std::stof(tokens[8]);

            auto& transform = view.get<TransformComponent>(entity);
            auto& rb = view.get<RigidbodyComponent>(entity);
            auto& input = view.get<InputComponent>(entity);
            auto& camera = view.get<CameraComponent>(entity);
            auto& interpolation = view.get<InterpolationComponent>(entity);

            if (networked.peerID == localPeerID) {
                // Local player
                if (glm::length(transform.translation - serverPos) > 0.2f) {
                    transform.translation = glm::mix(transform.translation, serverPos, 0.1f);
                }
            } else {
                // Remote player
                // transform.translation = serverPos;
                // transform.rotation.y = yaw;
                input.yaw = yaw;
                input.pitch = pitch;
                rb.velocity = serverVel;

                interpolation.previousPos = transform.translation;
                interpolation.previousRot.y = transform.rotation.y;
                interpolation.targetPos = serverPos;
                interpolation.targetRot.y = yaw;
            }
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
