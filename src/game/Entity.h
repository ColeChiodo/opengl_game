#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include <iostream>
#include <utility>
#include "entt/entt.hpp"

#include "Scene.h"

class Entity {
public:
    Entity(entt::entity handle, Scene* scene)
        : entityHandle(handle), scene(scene) {}

    template<typename T, typename... Args>
    T& addComponent(Args&&... args) {
        if (hasComponent<T>()) {
            std::cerr << "Entity already has component" << std::endl;
        }
        return scene->registry.emplace<T>(entityHandle, std::forward<Args>(args)...);
    }

    template<typename T>
    T& getComponent() {
        if (!hasComponent<T>()) {
            std::cerr << "Entity does not have component" << std::endl;
        }
        return scene->registry.get<T>(entityHandle);
    }

    template<typename T>
    bool hasComponent() {
        return scene->registry.all_of<T>(entityHandle);
    }

    template<typename T>
    void removeComponent() {
        if (!hasComponent<T>()) {
            std::cerr << "Entity does not have component" << std::endl;
        }
        scene->registry.remove<T>(entityHandle);
    }

    operator entt::entity() const { return entityHandle; }

private:
    entt::entity entityHandle;
    Scene* scene;
};

#endif