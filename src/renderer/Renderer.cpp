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

    auto group = scene.registry.group<TransformComponent>(entt::get<ModelComponent>);

    for (auto entity : group) {
        auto& modelComp = group.get<ModelComponent>(entity);
        modelComp.model.UpdateMatrix();
        modelComp.model.Draw(shader, camera);
    }

    camera.UpdateMatrix(45.0f, 0.1f, 1000.0f);

    EndFrame();
}
