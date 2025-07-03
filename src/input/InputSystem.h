#pragma once

#include "game/Scene.h"
#include "core/Window.h"
#include "network/Client.h"
#include <GLFW/glfw3.h>

struct InputPacket {
    glm::vec3 moveDir;
    float yaw;
    float pitch;
    bool wantsToJump;
    bool wantsToFire;
    uint32_t timestamp;
};


class InputSystem {
    public:
        void Process(Scene& scene, float deltaTime, Window& winObj, Client& client);
};
