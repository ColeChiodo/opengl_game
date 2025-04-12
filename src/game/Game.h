#ifndef GAME_CLASS_H
#define GAME_CLASS_H

#include "Scene.h"
#include "Entity.h"
#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "renderer/Light.h"
#include "core/Window.h"
#include "input/Input.h"

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
        Input inputs;

        // You can later add more game states, variables, etc. (e.g. score, game state, etc.)
};

#endif