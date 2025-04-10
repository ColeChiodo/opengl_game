#ifndef WINDOW_CLASS_H
#define WINDOW_CLASS_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "renderer/Camera.h"

class Window {
    public:
        GLFWwindow* window;
        int width, height;
        const char* title;

        Window(int width, int height, const char* title);
        ~Window();

        void pollEvents() const;
        void swapBuffers() const;
        bool shouldClose() const;
        void processInput();
        void toggleFullscreen();

    private:
        bool isFullscreen;
        GLFWmonitor* monitor;
        int windowedX, windowedY, windowedWidth, windowedHeight, windowPosX, windowPosY;
};

#endif