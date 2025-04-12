#include "Game.h"
#include <iostream>

Game::Game(Renderer& renderer, Window& window)
    : renderer(renderer), window(window) {}

Game::~Game() {}

void Game::Init() {
    // Initialize The Scene

    // Player
    Camera camera(window.width, window.height, glm::vec3(0.0f, 1.5f, 0.0f));
    auto player = scene.CreateEntity("Player");
    player.addComponent<CameraComponent>(camera, window);
    player.getComponent<CameraComponent>().isPrimary = true;
    player.addComponent<ModelComponent>("models/makoto_p3/scene.gltf");
    player.getComponent<ModelComponent>().modelScale = glm::vec3(0.01f);
    player.getComponent<ModelComponent>().modelRotation = glm::vec3(90.0f, 0.0f, 180.0f);

    // Plane
    auto plane = scene.CreateEntity("Plane");
    plane.addComponent<PrimitiveComponent>();
    plane.getComponent<PrimitiveComponent>().primitive.SetColor(glm::vec3(0.78f, 0.467f, 0.0f));
    plane.getComponent<PrimitiveComponent>().primitive.generatePrimitive(PrimitiveType::Plane);
    plane.getComponent<TransformComponent>().translation = glm::vec3(0.0f, 0.0f, 0.0f);
    plane.getComponent<TransformComponent>().scale = glm::vec3(10.0f);

    // Sushi
    auto sushi = scene.CreateEntity("Sushi");
    sushi.addComponent<ModelComponent>("models/sushi_bar/scene.gltf");
    sushi.getComponent<ModelComponent>().modelScale = glm::vec3(0.2f);
    sushi.getComponent<ModelComponent>().modelPosition = glm::vec3(0.0f, 0.25, 0.0f);
    sushi.getComponent<TransformComponent>().translation = glm::vec3(0.0f, 0.0f, -3.0f);

    // Light
    auto light = scene.CreateEntity("Light");
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(5.0f, -5.0f, 5.0f);
    int lightType = 1;
    glm::vec3 lightDir = glm::vec3(-0.0f, -1.0f, -0.75f);
    light.addComponent<LightComponent>(Light(lightColor, lightPos, lightType, lightDir));

    renderer.SetLighting(scene);
}

void Game::Update(float deltaTime) {
    // Update game logic here (e.g. move objects, handle AI, collisions, etc.)
    float rotationSpeed = 0.5f; // degrees per second

    auto view = scene.registry.view<TransformComponent, TagComponent>();
    view.each([&](auto entity, TransformComponent& transformComp, TagComponent& tagComp) {
        if (tagComp.Tag == "Sushi") {
            transformComp.rotation.y += rotationSpeed * deltaTime;
        }
    });
}

void Game::Render() {
    auto view = scene.registry.view<TransformComponent, CameraComponent>();
    view.each([&](auto entity, TransformComponent& transformComp, CameraComponent& camComp) {
        if (camComp.isPrimary) {
            renderer.DrawScene(scene, camComp.camera);
        }
    });
}

void Game::processInput() {
    // Handle input (e.g., ESC to exit, move camera, etc.)
    // Here you can forward input processing to the window or camera
    if (glfwGetKey(window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window.window, true);
    }

    // TODO replace with input component
    auto view = scene.registry.view<TransformComponent, CameraComponent>();
    view.each([&](auto entity, TransformComponent& transformComp, CameraComponent& camComp) {
        if (camComp.isPrimary) {
            camComp.camera.Inputs(window.window);
        }
    });
}
