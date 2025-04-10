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
    for (auto& light : scene.GetLights()) {
        light.setUniforms(shader);
    }
}

void Renderer::DrawScene(Scene& scene, Camera& camera) {
    for (auto& entity : scene.GetEntities()) {
        Model model = entity.model;
        model.UpdateMatrix();
        camera.UpdateMatrix(45.0f, 0.1f, 1000.0f);
        model.Draw(shader, camera);
    }
}
