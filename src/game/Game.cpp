#include "Game.h"
#include <iostream>

Game::Game(Renderer& renderer, Camera& camera, Window& window)
    : renderer(renderer), camera(camera), window(window) {}

Game::~Game() {}

void Game::Init() {
    // Initialize The Scene
    // Entity
    Model sushiModel("models/sushi_bar/scene.gltf");
    sushiModel.position = glm::vec3(0.0f, 0.0f, 0.0f);
    sushiModel.scale = glm::vec3(0.1f);

    Entity sushiEntity(sushiModel);
    scene.AddEntity(sushiEntity);

    // Light
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(5.0f, -5.0f, 5.0f);
    int lightType = 1;
    glm::vec3 lightDir = glm::vec3(-0.0f, -1.0f, -0.75f);

    Light light(lightColor, lightPos, lightType, lightDir);
    scene.AddLight(light);

    renderer.SetLighting(scene);
}

void Game::Update() {
    // Update game logic here (e.g. move objects, handle AI, collisions, etc.)
    for (auto& entity : scene.GetEntities()) {
        // You can rotate or update model positions here
        entity.model.rotation.y += 0.5f;  // For example, rotate each model
    }
}

void Game::Render() {
    renderer.DrawScene(scene, camera);
}

void Game::ProcessInput() {
    // Handle input (e.g., ESC to exit, move camera, etc.)
    // Here you can forward input processing to the window or camera
    if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window.window, true);
    }

    camera.Inputs(window.window);
}
