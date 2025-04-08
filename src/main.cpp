#include "Model.h"

const char* gameTitle = "Cole's Game";

unsigned int windowWidth = 1280;
unsigned int windowHeight = 720;

void windowInputs(GLFWwindow*);

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

	Shader shaderProgram("default.vert", "default.frag");

    glm::vec4 lightColor = glm::vec4(255.0f / 255.0f, 255.0f / 255.0f, 255.0f / 255.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(5.0f, -5.0f, 5.0f);
    int lightType = 1; // 0 = Point, 1 = Directional, 2 = Spotlight
    glm::vec3 lightDirection = glm::vec3(-0.0f, -1.0f, -0.75f);

    shaderProgram.Activate();
    glUniform4f(glGetUniformLocation(shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform1i(glGetUniformLocation(shaderProgram.ID, "lightType"), lightType);
    glUniform3f(glGetUniformLocation(shaderProgram.ID, "lightDir"), lightDirection.x, lightDirection.y, lightDirection.z);

	glEnable(GL_DEPTH_TEST);

	Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    Model model("models/sushi_bar/scene.gltf");

    model.position = glm::vec3(0.0f, 0.0f, 0.0f);
    model.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    model.scale = glm::vec3(0.1f);

    // For FPS Counter
    double prevTime = 0.0;
	double currTime = 0.0;
	double timeDiff;
	unsigned int counter = 0;

    // Main Loop
	while (!glfwWindowShouldClose(window)) {
        // FPS Counter
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

        // Game Update / Simulation
        model.rotation.y += 0.5;

        // Transform Updates
		camera.UpdateMatrix(45.0f, 0.1f, 1000.0f);
        model.UpdateMatrix();

        // Draw Events
        model.Draw(shaderProgram, camera);

        // Present Frame
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
