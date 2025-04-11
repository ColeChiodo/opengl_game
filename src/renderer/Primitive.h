#ifndef PRIMITIVE_CLASS_H
#define PRIMITIVE_CLASS_H

#include <optional>

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"

enum class PrimitiveType {
    Cube,
    Plane,
    Sphere
};

class Primitive {
    public:
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        Primitive();

        void Draw(Shader& shader, Camera& camera);
        void UpdateMatrix();
        void SetMatrix(glm::mat4 matrix);

        void generatePrimitive(PrimitiveType type);

        void SetTextures(const std::vector<Texture>& tex) {
            textures = tex;
            baseColor.reset();
        }
    
        void SetColor(const glm::vec3& color) {
            baseColor = color;
            textures.clear();
        }

    private:
        std::optional<Mesh> mesh;
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        std::vector<Texture> textures;
        std::optional<glm::vec3> baseColor;

        void generateCube();
        void generatePlane();
};

#endif