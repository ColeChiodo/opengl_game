#include "Primitive.h"

Primitive::Primitive() {}

void Primitive::generatePrimitive(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Cube:
            generateCube();
            break;
        case PrimitiveType::Plane:
            generatePlane();
            break;
    }
}

void Primitive::generatePlane() {
    glm::vec3 color = baseColor.value_or(glm::vec3(1.0f));
    Vertex vertices[] = {
     //             COORDINATES             /            COLORS          /           NORMALS          /  TEXTURE COORDINATES  //
        Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), color, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), color, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), color, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), color, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3,
    };

    std::vector<Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));

    mesh = Mesh(verts, ind, textures);
}

void Primitive::generateCube() {
    glm::vec3 color = baseColor.value_or(glm::vec3(1.0f));
    Vertex vertices[] = {
      //             COORDINATES             /            COLORS          /           NORMALS          /  TEXTURE COORDINATES  //
        Vertex{glm::vec3(-1.0f, -1.0f,  1.0f), color, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3( 1.0f, -1.0f,  1.0f), color, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3( 1.0f,  1.0f,  1.0f), color, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
        Vertex{glm::vec3(-1.0f,  1.0f,  1.0f), color, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(-1.0f, -1.0f, -1.0f), color, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
        Vertex{glm::vec3( 1.0f, -1.0f, -1.0f), color, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
        Vertex{glm::vec3( 1.0f,  1.0f, -1.0f), color, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
        Vertex{glm::vec3(-1.0f,  1.0f, -1.0f), color, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)}
    };
      
    GLuint indices[] = {
        0, 1, 2,  0, 2, 3,
        4, 6, 5,  4, 7, 6,
        4, 5, 1,  4, 1, 0,
        3, 2, 6,  3, 6, 7,
        1, 5, 6,  1, 6, 2,
        4, 0, 3,  4, 3, 7
    };

    std::vector<Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector<GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));

    mesh = Mesh(verts, ind, textures);
}

void Primitive::Draw(Shader& shader, Camera& camera) {
    if (mesh.has_value()) {
        shader.Activate();
        shader.setBool("useVertexColor", textures.empty());
        mesh->Draw(shader, camera, modelMatrix);
    }
}

void Primitive::UpdateMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    modelMatrix = glm::scale(modelMatrix, scale);
}

void Primitive::SetMatrix(glm::mat4 matrix) {
    modelMatrix = matrix;
}
