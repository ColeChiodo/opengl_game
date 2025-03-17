#include "meshClass.h"

const char* gameTitle = "Cole's Game";

unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;

void windowInputs(GLFWwindow*);

Vertex planeVertices[] = {
  //             COORDINATES             /            COLORS          /           NORMALS          /  TEXTURE COORDINATES  //
    Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3( 1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

GLuint planeIndices[] = {
    0, 1, 2,
	0, 2, 3,
};

Vertex turtleVertices[] = {
  //             COORDINATES             /            COLORS          /           NORMALS          /  TEXTURE COORDINATES  //
    Vertex{glm::vec3(-0.2f, -0.2f,  0.2f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
    Vertex{glm::vec3( 0.2f, -0.2f,  0.2f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f,  0.2f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
    Vertex{glm::vec3(-0.2f,  0.2f,  0.2f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
    Vertex{glm::vec3(-0.2f, -0.2f, -0.2f), glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
    Vertex{glm::vec3( 0.2f, -0.2f, -0.2f), glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
    Vertex{glm::vec3( 0.2f,  0.2f, -0.2f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
    Vertex{glm::vec3(-0.2f,  0.2f, -0.2f), glm::vec3(0.0f, 1.0f, 0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)}
};

GLuint turtleIndices[] = {
    0, 1, 2,  0, 2, 3,
    4, 6, 5,  4, 7, 6,
    4, 5, 1,  4, 1, 0,
    3, 2, 6,  3, 6, 7,
    1, 5, 6,  1, 6, 2,
    4, 0, 3,  4, 3, 7
};

Vertex lightVertices[] = {
  //             COORDINATES               //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] = {
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

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, gameTitle, NULL, NULL);
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

    Texture planeTextures[] {
		Texture("planks.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture("planksSpec.png", "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

    // Cube Object
	Shader shaderProgram("default.vert", "default.frag");

	std::vector <Vertex> planeVerts(planeVertices, planeVertices + sizeof(planeVertices) / sizeof(Vertex));
	std::vector <GLuint> planeInd(planeIndices, planeIndices + sizeof(planeIndices) / sizeof(GLuint));
	std::vector <Texture> planeTex(planeTextures, planeTextures + sizeof(planeTextures) / sizeof(Texture));

    Mesh plane(planeVerts, planeInd, planeTex);

    // Tutle Object
    Texture turtleTextures[] {
		Texture("turtle.png", "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE)
	};

    std::vector <Vertex> turtleVerts(turtleVertices, turtleVertices + sizeof(turtleVertices) / sizeof(Vertex));
	std::vector <GLuint> turtleInd(turtleIndices, turtleIndices + sizeof(turtleIndices) / sizeof(GLuint));
	std::vector <Texture> turtleTex(turtleTextures, turtleTextures + sizeof(turtleTextures) / sizeof(Texture));

    Mesh turtle(turtleVerts, turtleInd, turtleTex);

    // Light Object
    Shader lightShader("light.vert", "light.frag");

	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));

	Mesh light(lightVerts, lightInd, planeTex); // The light has no texture, so using the plane (as a default)

    glm::vec4 lightColor = glm::vec4(255.0f / 255.0f, 191.0f / 255.0f, 0.0f / 255.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(0.5f, -0.5f, 0.5f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    int lightType = 0; // 0 = Point, 1 = Directional, 2 = Spotlight
    glm::vec3 lightDirection = glm::vec3(-0.75f, -1.0f, -0.25f);
    lightModel = glm::translate(lightModel, lightPos);

    glm::vec3 planePos = glm::vec3(0.0f, -1.0f, 0.0f);
    glm::mat4 planeModel = glm::mat4(1.0f);
    planeModel = glm::translate(planeModel, planePos);

    glm::vec3 turtlePos = glm::vec3(0.0f, -0.8f, 0.0f);
    glm::mat4 turtleModel = glm::mat4(1.0f);
    turtleModel = glm::translate(turtleModel, turtlePos);

    // Pass Uniforms
    lightShader.Activate();
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform1i(glGetUniformLocation(lightShader.ID, "lightType"), lightType);
    glUniform3f(glGetUniformLocation(lightShader.ID, "lightDir"), lightDirection.x, lightDirection.y, lightDirection.z);
    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightType"), lightType);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightDir"), lightDirection.x, lightDirection.y, lightDirection.z);

	glEnable(GL_DEPTH_TEST);

	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    // For FPS Counter
    double prevTime = 0.0;
	double currTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

    // Main Loop
	while (!glfwWindowShouldClose(window)) {
        currTime = glfwGetTime();
		timeDiff = currTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 10.0) {
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = std::string(gameTitle) + " - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(window, newTitle.c_str());

			prevTime = currTime;
			counter = 0;
		}

        // Clear Screen
		glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Process Inputs
        windowInputs(window);
		camera.Inputs(window);

		camera.UpdateMatrix(45.0f, 0.1f, 100.0f);

        // Draw Events
        plane.Draw(shaderProgram, camera, planeModel);
        turtle.Draw(shaderProgram, camera, turtleModel);
		light.Draw(lightShader, camera, lightModel);

        // Process Events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	shaderProgram.Delete();
	lightShader.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	std::cout << "\nThanks For Playing!\n:]\n";
}

void windowInputs(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
