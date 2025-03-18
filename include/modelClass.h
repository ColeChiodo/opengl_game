#ifndef MODEL_CLASS_H
#define MODEL_CLASS_H

#include <json/json.h>
#include "meshClass.h"

using json = nlohmann::json;

class Model {
    public:
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 rotation = glm::vec3(0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
        
        Model(const char* file);

        void Draw(Shader& shader, Camera& camera);

        void UpdateMatrix();

    private:
        const char* file;
        std::vector<unsigned char> data;
        json JSON;

        std::vector<Mesh> meshes;
        std::vector<glm::vec3> translationsMeshes;
        std::vector<glm::quat> rotationsMeshes;
        std::vector<glm::vec3> scalesMeshes;
        std::vector<glm::mat4> matricesMeshes;

        std::vector<std::string> loadedTextureName;
        std::vector<Texture> loadedTexture;

        void loadMesh(unsigned int meshIndices);

        void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));

        std::vector<unsigned char> getData();
        std::vector<float> getFloats(json accessor);
        std::vector<GLuint> getIndices(json accessor);
        std::vector<Texture> getTextures();

        std::vector<Vertex> assembleVertices (std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texUVs);

        std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floatVec);
        std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floatVec);
        std::vector<glm::vec4> groupFloatsVec4(std::vector<float> floatVec);

        glm::mat4 modelMatrix = glm::mat4(1.0f);
};

#endif