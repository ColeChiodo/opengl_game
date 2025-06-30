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
    std::cout << "Connecting to " << host << " on port " << port << std::endl;

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

void Client::Send(const std::string& msg, MessageType type) {
    // Compose the message as: "type|payload"
    std::string packetData = std::to_string(type) + "|" + msg;

    ENetPacket* packet = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
    enet_host_flush(client);
}

void Client::Poll() {
    ENetEvent event;
    while (enet_host_service(client, &event, 0) > 0) {
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            std::string receivedMsg(reinterpret_cast<char*>(event.packet->data));
            // Parse the message type
            size_t separator = receivedMsg.find('|');
            if (separator != std::string::npos) {
                int type = std::stoi(receivedMsg.substr(0, separator));
                std::string payload = receivedMsg.substr(separator + 1);

                if (type == SEND_SCENE) {
                    std::cout << "Received Scene: " << payload << std::endl;
                    sceneReceivedCallback(payload);
                } else if (type == SEND_PLAYER_SPAWN) {
                    std::cout << "Received Player Spawn: " << payload << std::endl;
                    spawnNewPlayerCallback(payload == "true");
                }
            }
            enet_packet_destroy(event.packet);
        }
    }
}

void Client::RequestScene() {
    Send("", REQUEST_SCENE);
}

void Client::RequestPlayerSpawn() {
    Send("", REQUEST_PLAYER_SPAWN);
}

void Client::SetSceneReceivedCallback(std::function<void(const std::string&)> callback) {
    sceneReceivedCallback = callback;
}

void Client::SetSpawnNewPlayerCallback(std::function<void(const bool isClient)> callback) {
    spawnNewPlayerCallback = callback;
}
