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
            sceneData += tag.Tag + " " +std::to_string(id.ID) + "\n";
        }
    }

    return sceneData;
}
