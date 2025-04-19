#pragma once

#include "enet/enet.h"
#include <iostream>

class Server {
    public:
        Server(char* ipAddress, int port);
        ~Server();

        void Run();

    private:
        ENetHost* server = nullptr;
};