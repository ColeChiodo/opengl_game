#include "Renderer.h"
#include <iostream>

Renderer::Renderer()
    : shader("default.vert", "default.frag") {}

Renderer::~Renderer() {
    shader.Delete();
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

    auto view = scene.registry.view<TransformComponent, ModelComponent>();

    view.each([&](auto entity, TransformComponent& transform, ModelComponent& modelComp) {
        glm::mat4 worldTransform = transform.GetTransform();
        glm::mat4 modelLocalTransform = modelComp.GetLocalTransform();
    
        glm::mat4 finalTransform = worldTransform * modelLocalTransform;
    
        modelComp.model.SetMatrix(finalTransform);
        modelComp.model.Draw(shader, camera);
    });

    camera.UpdateMatrix(45.0f, 0.1f, 1000.0f);

    EndFrame();
}
