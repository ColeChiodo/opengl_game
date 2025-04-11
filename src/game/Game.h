#ifndef GAME_CLASS_H
#define GAME_CLASS_H

#include "Scene.h"
#include "Entity.h"
#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "renderer/Light.h"
#include "core/Window.h"

class Game {
    public:
        Game(Renderer& renderer, Camera& camera, Window& window);
        ~Game();

        void Init();
        void Update();
        void Render();
        void processInput();

    private:
        Renderer& renderer;
        Camera& camera;
        Window& window;
        Scene scene;

        // You can later add more game states, variables, etc. (e.g. score, game state, etc.)
};

#endif