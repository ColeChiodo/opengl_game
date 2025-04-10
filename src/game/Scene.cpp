#include "Scene.h"

Scene::Scene() {}

void Scene::AddEntity(const Entity& entity) {
    entities.push_back(entity);
}

std::vector<Entity>& Scene::GetEntities() {
    return entities;
}

void Scene::AddLight(const Light& light) {
    lights.push_back(light);
}

std::vector<Light>& Scene::GetLights() {
    return lights;
}