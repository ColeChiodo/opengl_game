#pragma once

#include "network/MessageTypes.h"
#include "Scene.h"
#include "enet/enet.h"
#include <iostream>
#include <thread>

class Server {
    public:
        Server(char* ipAddress, int port, Scene* sceneRef);
        ~Server();

        void Run();
        void HandleEvent(ENetEvent& event);
        
        void BroadcastScene(ENetPeer* peer);
        void BroadcastPlayerSpawn(ENetPeer* peer);

        int getPeerID(ENetPeer* peer);

    private:
        ENetHost* server = nullptr;
        Scene* scene = nullptr;
};