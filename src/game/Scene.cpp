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

std::string Scene::Serialize() {
    std::string sceneData;
    
    auto view = registry.view<TransformComponent, TagComponent, IDComponent>();
    
    for (auto entity : view) {
        auto& transform = view.get<TransformComponent>(entity);
        auto& tag = view.get<TagComponent>(entity);
        auto& id = view.get<IDComponent>(entity);

        sceneData += "EntityStart:\n";
        sceneData += "  ID: " + std::to_string(id.ID) + "\n";
        sceneData += "  Tag: " + tag.Tag + "\n";
        sceneData += "  Transform: " + transform.Serialize() + "\n";

        if (registry.any_of<ModelComponent>(entity)) {
            auto& model = registry.get<ModelComponent>(entity);
            sceneData += "  Model: " + model.Serialize() + "\n";
        }
        if (registry.any_of<PrimitiveComponent>(entity)) {
            auto& primitive = registry.get<PrimitiveComponent>(entity);
            //sceneData += "  Primitive: " + primitive.Serialize() + "\n";
        }
        if (registry.any_of<LightComponent>(entity)) {
            auto& light = registry.get<LightComponent>(entity);
            //sceneData += "  Light: " + light.Serialize() + "\n";
        }
        if (registry.any_of<CameraComponent>(entity)) {
            auto& camera = registry.get<CameraComponent>(entity);
            //sceneData += "  Camera: " + camera.Serialize() + "\n";
        }
        if (registry.any_of<InputComponent>(entity)) {
            auto& input = registry.get<InputComponent>(entity);
            //sceneData += "  Input: " + input.Serialize() + "\n";
        }
        if (registry.any_of<RigidbodyComponent>(entity)) {
            auto& rigidbody = registry.get<RigidbodyComponent>(entity);
            //sceneData += "  Rigidbody: " + rigidbody.Serialize() + "\n";
        }
        if (registry.any_of<BoxColliderComponent>(entity)) {
            auto& collider = registry.get<BoxColliderComponent>(entity);
            //sceneData += "  BoxCollider: " + collider.Serialize() + "\n";
        }

        sceneData += "EntityEnd\n";
    }

    return sceneData;
}
