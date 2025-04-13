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

        // You can later add more game states, variables, etc. (e.g. score, game state, etc.)
};

#endif