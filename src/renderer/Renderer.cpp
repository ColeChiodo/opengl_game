#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
    : shader("shaders/default.vert", "shaders/default.frag") {}

Renderer::~Renderer() {
    shader.Delete();
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void Renderer::Init() {
    shader.Activate();
}

void Renderer::BeginFrame() {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame() {
    // If needed for postprocessing or extra rendering tasks
}

void Renderer::SetLighting(Scene& scene) {
    auto view = scene.registry.view<LightComponent>();

    for (auto entity : view) {
        auto& lightComp = view.get<LightComponent>(entity);
        lightComp.light.setUniforms(shader);
    }
}

void Renderer::DrawScene(Scene& scene, Camera& camera) {
    BeginFrame();

    UpdateCameraMatrix(scene);

    shader.setBool("useVertexColor", false);
    DrawModels(scene, camera);

    shader.setBool("useVertexColor", true);
    DrawPrimitives(scene, camera);

    EndFrame();
}

void Renderer::DrawModels(Scene& scene, Camera& camera) {
    auto view = scene.registry.view<TransformComponent, ModelComponent>();

    view.each([&](auto entity, TransformComponent& transform, ModelComponent& modelComp) {
        glm::mat4 worldTransform = transform.GetTransform();
        glm::mat4 modelLocalTransform = modelComp.GetLocalTransform();
        glm::mat4 finalTransform = worldTransform * modelLocalTransform;

        modelComp.model.SetMatrix(finalTransform);
        modelComp.model.Draw(shader, camera);
    });
}

void Renderer::DrawPrimitives(Scene& scene, Camera& camera) {
    auto view = scene.registry.view<TransformComponent, PrimitiveComponent>();

    view.each([&](auto entity, TransformComponent& transform, PrimitiveComponent& primitiveComp) {
        glm::mat4 worldTransform = transform.GetTransform();
        glm::mat4 primitiveLocalTransform = primitiveComp.GetLocalTransform();
        glm::mat4 finalTransform = worldTransform * primitiveLocalTransform;

        primitiveComp.primitive.SetMatrix(finalTransform);
        primitiveComp.primitive.Draw(shader, camera);
    });
}

void Renderer::UpdateCameraMatrix(Scene& scene) {
    auto view = scene.registry.view<TransformComponent, CameraComponent>();

    view.each([&](auto entity, TransformComponent& transform, CameraComponent& camComp) {
        if (camComp.isPrimary) {
            camComp.camera.UpdateMatrix(0.1f, 1000.0f, transform.translation, transform.rotation);
        }
    });
}