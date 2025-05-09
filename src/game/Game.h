#ifndef GAME_CLASS_H
#define GAME_CLASS_H

#include "Scene.h"
#include "Entity.h"
#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "renderer/Light.h"
#include "core/Window.h"
#include "input/InputSystem.h"
#include "physics/RigidbodySystem.h"
#include "physics/BoxColliderSystem.h"

#include <thread>
#include <network/Server.h>
#include <network/Client.h>

void startServer();
void startClient();

enum class GameState {
    MainMenu,
    ConnectionMenu,
    Playing,
    Paused
};

class Game {
    public:
        Game(Renderer& renderer, Window& window);
        ~Game();

        void Init();
        void Update(float deltaTime);
        void Render();

    private:
        Renderer& renderer;
        Window& window;

        Scene scene;
        InputSystem inputs;
        RigidbodySystem rigidbodies;
        BoxColliderSystem colliders;

        GameState currentState = GameState::MainMenu;

        void UpdateGameplay(float deltaTime);

        void RenderMainMenu();
        void RenderConnectionMenu();
        void RenderGameplay();
};

#endif