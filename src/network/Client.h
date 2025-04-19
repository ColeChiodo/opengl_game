#pragma once

#include "enet/enet.h"
#include <iostream>

class Client {
    public:
        Client();
        ~Client();

        void Connect(const std::string& host, enet_uint16 port);
        void Send(const std::string& msg);

    private:
        ENetHost* client = nullptr;
        ENetPeer* peer = nullptr;
};