#ifndef SCENE_CLASS_H
#define SCENE_CLASS_H

#include <vector>
#include "Entity.h"
#include "renderer/Light.h"

class Scene {
    public:
        std::vector<Entity> entities;
        std::vector<Light> lights;

        Scene();

        void AddEntity(const Entity& Entity);
        std::vector<Entity>& GetEntities();

        void AddLight(const Light& Light);
        std::vector<Light>& GetLights();
};

#endif