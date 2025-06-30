#pragma once

#include "network/MessageTypes.h"
#include "Scene.h"
#include "enet/enet.h"
#include <iostream>

class Server {
    public:
        Server(char* ipAddress, int port, Scene* sceneRef);
        ~Server();

        void Run();
        void BroadcastScene(ENetPeer* peer);
        void BroadcastPlayerSpawn(ENetPeer* peer);

    private:
        ENetHost* server = nullptr;
        Scene* scene = nullptr;
};