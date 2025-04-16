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