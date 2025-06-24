#pragma once

#include "network/MessageTypes.h"
#include "enet/enet.h"
#include <iostream>

class Client {
    public:
        Client();
        ~Client();

        void Connect(const std::string& host, enet_uint16 port);
        void Send(const std::string& msg, MessageType type);
        void Poll();
        void RequestScene();

    private:
        ENetHost* client = nullptr;
        ENetPeer* peer = nullptr;
};