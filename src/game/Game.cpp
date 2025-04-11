#include "Game.h"
#include <iostream>

Game::Game(Renderer& renderer, Camera& camera, Window& window)
    : renderer(renderer), camera(camera), window(window) {}

Game::~Game() {}

void Game::Init() {
    // Initialize The Scene

    // Plane
    auto plane = scene.CreateEntity("Plane");
    plane.addComponent<PrimitiveComponent>();

    std::vector<Texture> texList = {
        Texture("planks.png", "diffuse", 0),
		Texture("planksSpec.png", "specular", 1)
    };

    plane.getComponent<PrimitiveComponent>().primitive.SetTextures(texList);
    plane.getComponent<PrimitiveComponent>().primitive.generatePrimitive(PrimitiveType::Plane);
    plane.getComponent<TransformComponent>().scale = glm::vec3(3.0f);

    // Sushi
    auto sushi = scene.CreateEntity("Sushi");
    sushi.addComponent<ModelComponent>("models/makoto_p3/scene.gltf");
    sushi.getComponent<ModelComponent>().modelScale = glm::vec3(0.01f);
    sushi.getComponent<ModelComponent>().modelRotation = glm::vec3(90.0f, 0.0f, 0.0f);
    sushi.getComponent<ModelComponent>().modelPosition = glm::vec3(0.0f, 0.0f, 0.0f);

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
    float rotationSpeed = 5.0f; // degrees per second

    auto view = scene.registry.view<TransformComponent, TagComponent>();
    view.each([&](auto entity, TransformComponent& transformComp, TagComponent& tagComp) {
        if (tagComp.Tag == "Sushi") {
            transformComp.rotation.y += rotationSpeed * deltaTime;
        }
    });
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
