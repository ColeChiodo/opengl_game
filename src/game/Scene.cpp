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
    // Format: "STATE, posX, posY, posZ, velX, velY, velZ, yaw, pitch"
    std::vector<std::string> tokens;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ' ')) {
        tokens.push_back(token);
    }

    auto view = registry.view<NetworkedComponent, InputComponent, TransformComponent, CameraComponent, RigidbodyComponent>();
    for (auto entity : view) {
        auto& networked = view.get<NetworkedComponent>(entity);
        if (networked.peerID == peerID && networked.peerID != localPeerID) {
            auto& transform = view.get<TransformComponent>(entity);
            auto& rb = view.get<RigidbodyComponent>(entity);
            auto& input = view.get<InputComponent>(entity);
            transform.translation.x = std::stof(tokens[1]);
            transform.translation.y = std::stof(tokens[2]);
            transform.translation.z = std::stof(tokens[3]);
            
            rb.velocity.x = std::stof(tokens[4]);
            rb.velocity.y = std::stof(tokens[5]);
            rb.velocity.z = std::stof(tokens[6]);
            
            transform.rotation.y = std::stof(tokens[7]);
            input.yaw = std::stof(tokens[7]);
            input.pitch = std::stof(tokens[8]);
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
