#include "Game.h"
#include <iostream>

void newImGuiFrame();
void drawImGui(Window& window);

Game::Game(Renderer& renderer, Window& window)
    : renderer(renderer), window(window) {}

Game::~Game() {}

void Game::Init() {
    // Initialize The Scene

    // Player
    auto player = scene.CreateEntity("Player");
    player.getComponent<TransformComponent>().translation = glm::vec3(0.0f, 0.0f, 0.0f);
    Camera camera(window.width, window.height, glm::vec3(0.0f, 1.75f, 0.0f));
    player.addComponent<CameraComponent>(camera, window);
    player.getComponent<CameraComponent>().isPrimary = true;
    player.addComponent<InputComponent>();
    player.getComponent<InputComponent>().enabled = true;
    // player.addComponent<ModelComponent>("models/makoto_p3/scene.gltf");
    // player.getComponent<ModelComponent>().modelRotation = glm::vec3(90.0f, 0.0f, 0.0f);
    // player.getComponent<ModelComponent>().modelPosition = glm::vec3(0.0f, -1.0f, 0.0f);
    // player.getComponent<ModelComponent>().modelScale = glm::vec3(0.0175f);
    player.addComponent<RigidbodyComponent>();
    player.addComponent<BoxColliderComponent>();
    player.getComponent<BoxColliderComponent>().size = glm::vec3(0.5f, 1.0f, 0.5f);

    // Player 2
    auto player2 = scene.CreateEntity("Player");
    player2.getComponent<TransformComponent>().translation = glm::vec3(-2.0f, 0.0f, 5.0f);
    player2.getComponent<TransformComponent>().rotation = glm::vec3(0.0f, 180.0f, 0.0f);
    Camera camera2(window.width, window.height, glm::vec3(0.0f, 1.75f, 0.0f));
    player2.addComponent<CameraComponent>(camera, window);
    player2.addComponent<InputComponent>();
    player2.addComponent<ModelComponent>("models/makoto_p3/scene.gltf");
    player2.getComponent<ModelComponent>().modelRotation = glm::vec3(90.0f, 0.0f, 0.0f);
    player2.getComponent<ModelComponent>().modelPosition = glm::vec3(0.0f, -1.0f, 0.0f);
    player2.getComponent<ModelComponent>().modelScale = glm::vec3(0.0175f);
    player2.addComponent<RigidbodyComponent>();
    player2.addComponent<BoxColliderComponent>();
    player2.getComponent<BoxColliderComponent>().size = glm::vec3(0.5f, 1.0f, 0.5f);

    // Cube
    auto cube = scene.CreateEntity("Cube");
    cube.getComponent<TransformComponent>().translation = glm::vec3(2.0f, 1.0f, 6.0f);
    cube.addComponent<PrimitiveComponent>();
    cube.getComponent<PrimitiveComponent>().primitive.generatePrimitive(PrimitiveType::Cube);
    cube.addComponent<BoxColliderComponent>();
    cube.getComponent<BoxColliderComponent>().isStatic = true;

    // Plane / Ground
    auto plane = scene.CreateEntity("Plane");
    plane.getComponent<TransformComponent>().translation = glm::vec3(0.0f, -0.01f, 0.0f);
    plane.getComponent<TransformComponent>().scale = glm::vec3(20.0f, 0.0f, 20.0f);
    plane.addComponent<PrimitiveComponent>();
    plane.getComponent<PrimitiveComponent>().primitive.SetColor(glm::vec3(0.78f, 0.467f, 0.0f));
    plane.getComponent<PrimitiveComponent>().primitive.generatePrimitive(PrimitiveType::Plane);
    plane.addComponent<BoxColliderComponent>();
    plane.getComponent<BoxColliderComponent>().isStatic = true;
    plane.getComponent<BoxColliderComponent>().size = glm::vec3(20.0f, 0.1f, 20.0f);
    plane.getComponent<BoxColliderComponent>().offset = glm::vec3(0.0f, -0.1f, 0.0f);

    // Slope
    auto slope = scene.CreateEntity("Plane");
    slope.getComponent<TransformComponent>().translation = glm::vec3(0.0f, 1.0f, 23.0f);
    slope.getComponent<TransformComponent>().rotation = glm::vec3(-10.0f, 0.00f, 0.0f);
    slope.getComponent<TransformComponent>().scale = glm::vec3(10.0f, 0.0f, 10.0f);
    slope.addComponent<PrimitiveComponent>();
    slope.getComponent<PrimitiveComponent>().primitive.SetColor(glm::vec3(0.467f, 0.78f, 0.0f));
    slope.getComponent<PrimitiveComponent>().primitive.generatePrimitive(PrimitiveType::Plane);
    slope.addComponent<BoxColliderComponent>();
    slope.getComponent<BoxColliderComponent>().isStatic = true;
    slope.getComponent<BoxColliderComponent>().size = glm::vec3(10.0f, 0.1f, 10.0f);


    // Sushi
    auto sushi = scene.CreateEntity("Sushi");
    sushi.getComponent<TransformComponent>().translation = glm::vec3(2.0f, 2.0f, 6.0f);
    sushi.addComponent<ModelComponent>("models/sushi_bar/scene.gltf");
    sushi.getComponent<ModelComponent>().modelScale = glm::vec3(0.2f);
    sushi.getComponent<ModelComponent>().modelPosition = glm::vec3(0.0f, 0.25, 0.0f);

    // Light
    auto light = scene.CreateEntity("Light");
    glm::vec4 lightColor = glm::vec4(1.0f);
    glm::vec3 lightPos = glm::vec3(5.0f, -5.0f, 5.0f);
    int lightType = 1;
    glm::vec3 lightDir = glm::vec3(-0.0f, -1.0f, -0.75f);
    light.addComponent<LightComponent>(Light(lightColor, lightPos, lightType, lightDir));
}

void Game::Update(float deltaTime) {
    switch (currentState) {
        case GameState::Playing:
            UpdateGameplay(deltaTime);
            break;
        default:
            break;
    }
}

void Game::Render() {
    newImGuiFrame();

    switch (currentState) {
        case GameState::MainMenu:
            RenderMainMenu();
            break;
        case GameState::ConnectionMenu:
            RenderConnectionMenu();
            break;
        case GameState::Playing:
            RenderGameplay();
            break;
        default:
            break;
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::UpdateGameplay(float deltaTime) {
    // Process Inputs
    inputs.Process(scene, deltaTime, window);

    // Process Physics
    rigidbodies.Process(scene, deltaTime);
    colliders.Process(scene, deltaTime);

    // World Object Events
    float rotationSpeed = 30.0f; // degrees per second

    auto view = scene.registry.view<TransformComponent, TagComponent>();

    view.each([&](auto entity, TransformComponent& transform, TagComponent& tag) {
        if (tag.Tag == "Sushi") {
            transform.rotation.y += rotationSpeed * deltaTime;
        }
    });

    renderer.SetLighting(scene);
}

void Game::RenderGameplay() {
    auto view = scene.registry.view<TransformComponent, CameraComponent>();
    view.each([&](auto entity, TransformComponent& transformComp, CameraComponent& camComp) {
        if (camComp.isPrimary) {
            renderer.DrawScene(scene, camComp.camera);
        }
    });

    ImGuiIO& io = ImGui::GetIO();

    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window.window, &framebufferWidth, &framebufferHeight);
    io.DisplaySize = ImVec2((float)framebufferWidth, (float)framebufferHeight);

    // Optional background text overlay
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList();
    ImU32 color = IM_COL32(255, 255, 0, 255);
    float fontSize = 20.0f;
    ImFont* font = ImGui::GetFont();

    draw_list->AddText(font, fontSize, ImVec2(0, 0), color, glfwGetWindowTitle(window.window));
    draw_list->AddText(font, fontSize, ImVec2(framebufferWidth - 100, 0), color, "Top Right");
    draw_list->AddText(font, fontSize, ImVec2(0, framebufferHeight - 20), color, "Bottom Left");
    draw_list->AddText(font, fontSize, ImVec2(framebufferWidth - 130, framebufferHeight - 20), color, "Bottom Right");
}

void Game::RenderMainMenu() {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize(200, 130);  // Slightly taller to fit vertical buttons
    ImVec2 center((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);
    ImGui::SetNextWindowPos(center);
    ImGui::SetNextWindowSize(windowSize);

    ImGui::Begin("Main Menu", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground
    );

    ImVec2 buttonSize(100, 30); // Wider buttons for better appearance
    float buttonSpacing = 15.0f;

    // Calculate X position to center buttons
    float buttonX = (windowSize.x - buttonSize.x) * 0.5f;

    float topPadding = 30.0f;
    ImGui::SetCursorPosY(topPadding);

    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Play", buttonSize)) {
        currentState = GameState::ConnectionMenu;
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSpacing);
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Quit", buttonSize)) {
        glfwSetWindowShouldClose(window.window, true);
    }

    ImGui::End();
}

void Game::RenderConnectionMenu() {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGuiIO& io = ImGui::GetIO();
    ImVec2 windowSize(300, 200);
    ImVec2 center((io.DisplaySize.x - windowSize.x) * 0.5f, (io.DisplaySize.y - windowSize.y) * 0.5f);
    ImGui::SetNextWindowPos(center);
    ImGui::SetNextWindowSize(windowSize);

    ImGui::Begin("Connection", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground
    );

    ImVec2 buttonSize(100, 30); // slightly wider
    float buttonSpacing = 15.0f;

    // Calculate X position to center buttons
    float buttonX = (windowSize.x - buttonSize.x) * 0.5f;

    // Add space from top if you want it more visually centered
    float topPadding = 30.0f;
    ImGui::SetCursorPosY(topPadding);

    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Host", buttonSize)) {
        startServer();
        Init();
        currentState = GameState::Playing;
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSpacing);
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Client", buttonSize)) {
        startClient();
        Init();
        currentState = GameState::Playing;
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSpacing);
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Back", buttonSize)) {
        currentState = GameState::MainMenu;
    }

    ImGui::End();
}

void newImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void startServer() {
    Client client;

    // Start server in background
    std::thread([]() {
        Server host(nullptr, 1111);
        host.Run();
    }).detach();

    // Delay slightly to let the server fully start before connecting. bad, swap to "while not started" loop
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    client.Connect("127.0.0.1", 1111);
    client.Send("Hello from host/client!");
}

void startClient() {
    Client client;
    client.Connect("127.0.0.1", 1111);
    client.Send("Hello from client!");
}