#include "Server.h"

Server::Server(char* ipAddress, int port) {
    if (enet_initialize() != 0) {
        std::cerr << "ENet initialization failed.\n";
        exit(EXIT_FAILURE);
    }
    atexit(enet_deinitialize);

    ENetAddress address;

    if (ipAddress == NULL) {
        address.host = ENET_HOST_ANY;
    } else {
        enet_address_set_host(&address, ipAddress);
    }
    address.port = port;

    server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server) {
        std::cerr << "Failed to create ENet server.\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " << port << std::endl;
}

Server::~Server() {
    if (server) {
        enet_host_destroy(server);
    }
}

void Server::Run() {
    ENetEvent event;
    while (true) {
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "Client connected.\n";
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    std::cout << "Packet received: " << (char*)event.packet->data << "\n";
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Client disconnected.\n";
                    break;
                default:
                    break;
            }
        }
    }
}