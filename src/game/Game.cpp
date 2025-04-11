#include "Game.h"
#include <iostream>

Game::Game(Renderer& renderer, Camera& camera, Window& window)
    : renderer(renderer), camera(camera), window(window) {}

Game::~Game() {}

void Game::Init() {
    // Initialize The Scene

    // Sushi
    auto sushi = scene.CreateEntity("Sushi");
    sushi.addComponent<ModelComponent>("models/sushi_bar/scene.gltf");

    sushi.getComponent<ModelComponent>().model.scale = glm::vec3(0.1f);

    // Light
    auto light = scene.CreateEntity("Light");
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(5.0f, -5.0f, 5.0f);
    int lightType = 1;
    glm::vec3 lightDir = glm::vec3(-0.0f, -1.0f, -0.75f);
    light.addComponent<LightComponent>(Light(lightColor, lightPos, lightType, lightDir));

    renderer.SetLighting(scene);
}

void Game::Update() {
    // Update game logic here (e.g. move objects, handle AI, collisions, etc.)
    auto view = scene.registry.view<ModelComponent>();

    for (auto entity : view) {
        auto& modelComp = view.get<ModelComponent>(entity);
        modelComp.model.rotation.y += 0.5f;
    }
}

void Game::Render() {
    renderer.DrawScene(scene, camera);
}

void Game::processInput() {
    // Handle input (e.g., ESC to exit, move camera, etc.)
    // Here you can forward input processing to the window or camera
    if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window.window, true);
    }

    camera.Inputs(window.window);
}
