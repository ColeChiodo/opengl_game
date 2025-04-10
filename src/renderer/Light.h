#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include <glm/glm.hpp>

#include "Shader.h"

class Light {
    public:
        glm::vec4 lightColor = glm::vec4(1.0f);
        glm::vec3 lightPos = glm::vec3(5.0f, -5.0f, 5.0f);
        int lightType = 1;
        glm::vec3 lightDir = glm::vec3(-0.0f, -1.0f, -0.75f);

        Light(glm::vec4 lightColor, glm::vec3 lightPos, int lightType, glm::vec3 lightDir);

        void setUniforms(Shader& shader);
};

#endif