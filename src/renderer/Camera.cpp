#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position) {
	Camera::width = width;
	Camera::height = height;
	Camera::Position = position;
}

void Camera::UpdateMatrix(float FOVdeg, float nearPlane, float farPlane, glm::vec3 worldPosition, glm::vec3 worldRotation) {
    glm::vec3 finalPosition = Position + worldPosition;
    glm::vec3 finalOrientation = Orientation + worldRotation;
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(finalPosition, finalPosition + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

    cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform) {
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

glm::vec3 Camera::GetForward() const {
    glm::vec3 flatForward = Orientation;
    flatForward.y = 0.0f;
    return glm::normalize(flatForward);
}
