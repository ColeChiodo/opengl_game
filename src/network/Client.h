#pragma once

#include "network/MessageTypes.h"
#include "enet/enet.h"
#include <iostream>
#include <functional>

class Client {
    public:
        Client();
        ~Client();

        void Connect(const std::string& host, enet_uint16 port);
        void Send(const std::string& msg, MessageType type);
        void Poll();
        void RequestScene();
        void RequestPlayerSpawn();

        void SetSceneReceivedCallback(std::function<void(const std::string&)> callback);
        void SetSpawnNewPlayerCallback(std::function<void(const bool isClient, const int peerID)> callback);
        void SetUpdatePlayerStateCallback(std::function<void(const int peerID, const std::string& input)> callback);

        int GetLocalPeerID() { return localPeerID; }
        void setLocalPeerID(int peerID) { localPeerID = peerID; }

    private:
        ENetHost* client = nullptr;
        ENetPeer* peer = nullptr;

        int localPeerID = -1;

        std::function<void(const std::string&)> sceneReceivedCallback;
        std::function<void(const bool isClient, const int peerID)> spawnNewPlayerCallback;
        std::function<void(const int peerID, const std::string& input)> updatePlayerStateCallback;
};
