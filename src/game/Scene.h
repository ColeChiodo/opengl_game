#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <string>

#include "entt/entt.hpp"

#include "Components.h"

class Entity;

class Scene {
    public:
        Scene();
        ~Scene();

        Entity CreateEntity(const std::string& name);

        entt::registry registry;

    private:
        
};

#endif