#include "Client.h"

Client::Client() {
    if (enet_initialize() != 0) {
        std::cerr << "ENet initialization failed.\n";
        exit(EXIT_FAILURE);
    }
    atexit(enet_deinitialize);

    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client) {
        std::cerr << "Failed to create ENet client.\n";
        exit(EXIT_FAILURE);
    }
}

Client::~Client() {
    if (client) enet_host_destroy(client);
}

void Client::Connect(const std::string& host, enet_uint16 port) {
    ENetAddress address;
    ENetEvent event;

    enet_address_set_host(&address, host.c_str());
    address.port = port;

    peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        std::cerr << "No available peers for initiating connection.\n";
        exit(EXIT_FAILURE);
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connected to server.\n";
    } else {
        std::cerr << "Connection to server failed.\n";
        enet_peer_reset(peer);
    }
}

void Client::Send(const std::string& msg) {
    ENetPacket* packet = enet_packet_create(msg.c_str(), msg.length() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);
}