#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "shaderClass.h"
#include "textureClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "camera.h"

unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;

void windowInputs(GLFWwindow*);

float planeVertices[] = {
    //     COORDINATES     /        COLORS          /    TexCoord   /        NORMALS       //
	-1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
	-1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
	 1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f

};

unsigned int planeIndices[] = {
    0, 1, 2,
	0, 2, 3,
};

float lightVertices[] = {
    -0.1f, -0.1f,  0.1f,
     0.1f, -0.1f,  0.1f,
     0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f,  0.1f, -0.1f,
    -0.1f,  0.1f, -0.1f,
};

unsigned int lightIndices[] = {
    0, 1, 2,
    0, 2, 3,
    4, 5, 6,
    4, 6, 7,
    4, 0, 3,
    4, 3, 7,
    1, 5, 6,
    1, 6, 2,
    4, 5, 1,
    4, 1, 0,
    3, 2, 6,
    3, 6, 7
};

int main() {
	if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "My Game", NULL, NULL);
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
    glViewport(0, 0, windowWidth, windowHeight);

	if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Cube Object
	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(planeVertices, sizeof(planeVertices));
	EBO EBO1(planeIndices, sizeof(planeIndices));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));
    VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	EBO1.Unbind();

    // Light Object
    Shader lightShader("light.vert", "light.frag");

	VAO lightVAO;
	lightVAO.Bind();

	VBO lightVBO(lightVertices, sizeof(lightVertices));
	EBO lightEBO(lightIndices, sizeof(lightIndices));

	lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

	lightVAO.Unbind();
	lightVBO.Unbind();
	lightEBO.Unbind();

    glm::vec4 lightColor = glm::vec4(255.0f / 255.0f, 191.0f / 255.0f, 0.0f / 255.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, -0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    int lightType = 2; // 0 = Point, 1 = Directional, 2 = Spotlight
    glm::vec3 lightDirection = glm::vec3(-0.75f, -1.0f, -0.25f);
    lightModel = glm::translate(lightModel, lightPos);

    glm::vec3 cubePos = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::mat4 cubeModel = glm::mat4(1.0f);
    cubeModel = glm::translate(cubeModel, cubePos);

    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform1i(glGetUniformLocation(lightShader.ID, "lightType"), lightType);
    glUniform3f(glGetUniformLocation(lightShader.ID, "lightDir"), lightDirection.x, lightDirection.y, lightDirection.z);
    shaderProgram.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(cubeModel));
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightType"), lightType);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightDir"), lightDirection.x, lightDirection.y, lightDirection.z);

    Texture plankTexture("turtle.png", GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE);
	plankTexture.texUnit(shaderProgram, "tex0", 0);
    Texture plankSpecular("planksSpec.png", GL_TEXTURE_2D, 1, GL_RED, GL_UNSIGNED_BYTE);
    plankTexture.texUnit(shaderProgram, "tex1", 1);

	glEnable(GL_DEPTH_TEST);

	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    // Main Loop
	while (!glfwWindowShouldClose(window)) {
        // Clear Screen
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Process Inputs
        windowInputs(window);
		camera.Inputs(window);

		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        shaderProgram.Activate();
        glUniform3f(glGetUniformLocation(shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
        camera.Matrix(shaderProgram, "camMatrix");

        plankTexture.Bind();
        plankSpecular.Bind();
		VAO1.Bind();

		glDrawElements(GL_TRIANGLES, sizeof(planeIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        lightShader.Activate();
        camera.Matrix(lightShader, "camMatrix");

        lightVAO.Bind();

        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // Process Events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	VAO1.Delete();
	VBO1.Delete();
	EBO1.Delete();
    plankTexture.Delete();
	shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	std::cout << "\nThanks For Playing!\n:]\n";
}

void windowInputs(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
