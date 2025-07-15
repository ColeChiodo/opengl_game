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
        
        void BroadcastSetPlayerID(ENetPeer* peer);
        void BroadcastScene(ENetPeer* peer);
        void BroadcastPlayerSpawn(ENetPeer* peer);
        void BroadcastPlayerPositions();

        int getPeerID(ENetPeer* peer);

    private:
        ENetHost* server = nullptr;
        Scene* scene = nullptr;

        const int TICK_RATE = 60;
        const int MS_PER_TICK = 1000 / TICK_RATE;
};