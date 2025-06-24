#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <string>
#include <sstream>

#include "entt/entt.hpp"

#include "Components.h"

class Entity;

class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name);
        void DestroyEntity(entt::entity entity);

        std::string Serialize();

        entt::registry registry;

    private:
        uint64_t totalEntities = 0;
};

#endif