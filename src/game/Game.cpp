#include "Game.h"
#include <iostream>

void newImGuiFrame();
void drawImGui(Window& window);

Game::Game(Renderer& renderer, Window& window)
    : renderer(renderer), window(window) {
    client.SetSceneReceivedCallback([this](const std::string& sceneData) {
        // given scene data in format: levelName\nPlayer ID1\nPlayer ID2\n...
        std::string levelName = sceneData.substr(0, sceneData.find("\n"));
        this->LoadLevel(levelName);

        std::string peerIDs = sceneData.substr(sceneData.find("\n") + 1);
        std::stringstream ss(peerIDs);
        std::string peerID;
        // load players already in scene
        while (std::getline(ss, peerID, '\n')) {
            peerID = peerID.substr(7);
            std::cout << "[Client] Loading Player with Peer ID: " << peerID << std::endl;
            this->AddPlayer(false, std::stoi(peerID));
        }

        // request new player spawn
        client.RequestPlayerSpawn();
    });

    client.SetSpawnNewPlayerCallback([this](const bool isClient, const int peerID) {
        this->AddPlayer(isClient, peerID);
    });

    client.SetUpdatePlayerStateCallback([this](const int peerID, const std::string& input) {
        this->UpdatePlayerState(peerID, input);
    });
}

Game::~Game() {}

void Game::Init() {
    // Initialize The Scene
    LoadLevel("Test_Level");
    client.RequestPlayerSpawn();
}

void Game::LoadLevel(const std::string& levelName) {
    std::cout << "[Client] Loading Level: " << levelName << std::endl;
    // Test Level
    scene.CreateEntity("Test_Level");
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

    std::cout << "[Client] Loaded Level: " << levelName << std::endl;
}

void Game::AddPlayer(bool isClient, int peerID) {
    std::cout << "[Client] Adding Player " << isClient << " " << peerID << std::endl;
    
    auto player = scene.CreateEntity("Player");
    Camera camera(window.width, window.height, glm::vec3(0.0f, 1.75f, 0.0f));
    player.addComponent<CameraComponent>(camera, window);
    player.addComponent<InputComponent>();
    player.addComponent<RigidbodyComponent>();
    player.addComponent<BoxColliderComponent>();
    player.getComponent<BoxColliderComponent>().size = glm::vec3(0.5f, 1.0f, 0.5f);
    player.addComponent<NetworkedComponent>();
    player.getComponent<NetworkedComponent>().peerID = peerID;

    // if player is current client
    if (isClient) {
        player.getComponent<CameraComponent>().isPrimary = true;
        player.getComponent<InputComponent>().enabled = true;
    } else {
        player.addComponent<ModelComponent>("models/makoto_p3/scene.gltf");

        // Model adjustments for makoto_p3
        player.getComponent<ModelComponent>().modelRotation = glm::vec3(90.0f, 0.0f, 0.0f);
        player.getComponent<ModelComponent>().modelPosition = glm::vec3(0.0f, -1.0f, 0.0f);
        player.getComponent<ModelComponent>().modelScale = glm::vec3(0.0175f);
    }

    // adjust spawn location
    player.getComponent<TransformComponent>().translation = glm::vec3(0.0f, 0.0f, 0.0f);
}

void Game::UpdatePlayerState(int peerID, const std::string& input) {
    scene.UpdatePlayerState(peerID, input, client.GetLocalPeerID());
}

void Game::Update(float deltaTime) {
    client.Poll();
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
    inputs.Process(scene, deltaTime, window, client);

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
        //Init();
        currentState = GameState::Playing;
    }

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + buttonSpacing);
    ImGui::SetCursorPosX(buttonX);
    if (ImGui::Button("Client", buttonSize)) {
        startClient();
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

void Game::startServer() {
    // Start server in background
    std::thread([this]() {
        Server host(nullptr, 23456, &scene);
        host.Run();
    }).detach();

    // Delay slightly to let the server fully start before connecting. bad, swap to "while not started" loop
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // connect to local host port 23456
    client.Connect("127.0.0.1", 23456);
    client.Send("Hello from host/client!", CHAT_MESSAGE);

    // initialize the game. build the scene
    Init();
}

void Game::startClient() {
    client.Connect("127.0.0.1", 23456);
    client.Send("Hello from client!", CHAT_MESSAGE);

    // request the scene from the server
    std::cout << "[Client] Requesting scene from server.\n";
    client.RequestScene();
}
