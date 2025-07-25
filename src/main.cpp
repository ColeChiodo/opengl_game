#define ENET_IMPLEMENTATION
#include <chrono>

#include "core/Window.h"
#include "game/Game.h"
#include "renderer/Renderer.h"
#include "renderer/Camera.h"

#include <steam_api.h>

const char* gameTitle = "Cole's Game";

int main() {
    if (!SteamAPI_Init()) {
        std::cerr << "❌ SteamAPI_Init() failed! Is Steam running? Is steam_appid.txt in the right place?" << std::endl;
        return 1;
    }
    const char* name = SteamFriends()->GetPersonaName();
    std::cout << "✅ Steam initialized successfully!, logged in as: " << name << std::endl;

    Window appWindow(1280, 720, gameTitle);

    Renderer renderer;
    renderer.Init();
    
    Game game(renderer, appWindow);

    // FPS Counter Stuff
    double prevTime = 0.0;
    double currTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;

    // Delta time
    auto lastTime = std::chrono::high_resolution_clock::now();

    // Main game loop
    while (!appWindow.shouldClose()) {
        // Delta time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

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

        // Update game logic
        game.Update(deltaTime);

        // Render Frame
        game.Render();

        // Swap buffers and poll events
        appWindow.swapBuffers();
        appWindow.pollEvents();
    }

    std::cout << "Thanks For Playing My Game\n:]\n";

    SteamAPI_Shutdown();

    return 0;
}
