#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"

class Camera {
    public:
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 cameraMatrix = glm::mat4(1.0f);

        bool firstFocus = true;

        int width;
        int height;

        const float defaultSpeed = 0.05f;
        const float fastSpeed = 0.15f;
        float currSpeed = defaultSpeed;
        float sensitivity = 0.1f;

        Camera(int width, int height, glm::vec3 position);

        void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane, glm::vec3 worldPosition, glm::vec3 worldRotation);
        void Matrix(Shader& shader, const char* uniform);

        glm::vec3 GetForward() const;
};

#endif