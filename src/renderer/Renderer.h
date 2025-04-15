#ifndef RENDERER_CLASS_H
#define RENDERER_CLASS_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <GL/glew.h>
#include "Shader.h"
#include "Camera.h"
#include "game/Scene.h"

class Renderer {
    public:
        Shader shader;

        Renderer();
        ~Renderer();

        void Init();
        void SetLighting(Scene& scene);
        void DrawScene(Scene& scene, Camera& camera);

    private:
        void DrawModels(Scene& scene, Camera& camera);
        void DrawPrimitives(Scene& scene, Camera& camera);
        void UpdateCameraMatrix(Scene& scene);
        void BeginFrame();
        void EndFrame();
};

#endif