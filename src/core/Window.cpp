#include "Window.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);

    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    if (camera) {
        camera->width = width;
        camera->height = height;
    }
}

Window::Window(int width, int height, const char* title)
    : width(width), height(height), title(title), isFullscreen(false) {
    
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    monitor = glfwGetPrimaryMonitor();
    glfwGetWindowPos(window, &windowedX, &windowedY);
    glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        exit(-1);
    }

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 1280, 720);

    // set vsync (TODO make optional)
    glfwSwapInterval(1);
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::pollEvents() const {
    glfwPollEvents();
}

void Window::swapBuffers() const {
    glfwSwapBuffers(window);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::processInput() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    static bool f11PressedLastFrame = false;
    bool f11Pressed = glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;

    if (f11Pressed && !f11PressedLastFrame) {
        toggleFullscreen();
    }

    f11PressedLastFrame = f11Pressed;
}

void Window::toggleFullscreen() {
    isFullscreen = !isFullscreen;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (isFullscreen) {
        // Save current windowed position and size
        glfwGetWindowPos(window, &windowPosX, &windowPosY);
        glfwGetWindowSize(window, &windowedWidth, &windowedHeight);

        // Switch to fullscreen
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        // Switch back to windowed mode
        glfwSetWindowMonitor(window, nullptr, windowPosX, windowPosY, windowedWidth, windowedHeight, 0);
    }
}


