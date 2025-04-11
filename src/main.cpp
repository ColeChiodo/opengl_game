#include "core/Window.h"
#include "game/Game.h"
#include "renderer/Renderer.h"
#include "renderer/Camera.h"

const char* gameTitle = "Cole's Game";

int main() {
    Window appWindow(1280, 720, gameTitle);

    Renderer renderer;
    renderer.Init();

    Camera camera(appWindow.width, appWindow.height, glm::vec3(0.0f, 0.0f, 2.0f));
    glfwSetWindowUserPointer(appWindow.window, &camera);

    Game game(renderer, camera, appWindow);
    game.Init();

    // FPS Counter Stuff
    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;

    // Main game loop
    while (!appWindow.shouldClose()) {
        // FPS Counter Stuff
        currTime = glfwGetTime();
		timeDiff = currTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 10.0) {
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string ms = std::to_string((timeDiff / counter) * 1000);
			std::string newTitle = std::string(gameTitle) + " - " + FPS + "FPS / " + ms + "ms";
			glfwSetWindowTitle(appWindow.window, newTitle.c_str());

			prevTime = currTime;
			counter = 0;
		}

        // Handle input
        appWindow.processInput();
        game.processInput();

        // Update game logic
        game.Update();

        // Render Frame
        game.Render();

        // Swap buffers and poll events
        appWindow.swapBuffers();
        appWindow.pollEvents();
    }

    return 0;
}