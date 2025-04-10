#include "Light.h"

Light::Light(glm::vec4 lightColor, glm::vec3 lightPos, int lightType, glm::vec3 lightDir)
     : lightColor(lightColor), lightPos(lightPos), lightType(lightType), lightDir(lightDir) {}

void Light::setUniforms(Shader& shader) {
    glUniform4f(glGetUniformLocation(shader.ID, "lightColor"), lightColor.r, lightColor.g, lightColor.b, lightColor.a);
    glUniform3f(glGetUniformLocation(shader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform1i(glGetUniformLocation(shader.ID, "lightType"), lightType);
    glUniform3f(glGetUniformLocation(shader.ID, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
}