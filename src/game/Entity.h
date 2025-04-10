#ifndef ENTITY_CLASS_H
#define ENTITY_CLASS_H

#include "renderer/Model.h"

class Entity {
    public:
        // entity components

        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        Model model;

        Entity(Model& model);

    private:
};

#endif