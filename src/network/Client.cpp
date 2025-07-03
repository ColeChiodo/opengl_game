#include "Client.h"

Client::Client() {
    if (enet_initialize() != 0) {
        std::cerr << "[Client] ENet initialization failed.\n";
        exit(EXIT_FAILURE);
    }
    atexit(enet_deinitialize);

    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client) {
        std::cerr << "[Client] Failed to create ENet client.\n";
        exit(EXIT_FAILURE);
    }
}

Client::~Client() {
    if (client) enet_host_destroy(client);
}

void Client::Connect(const std::string& host, enet_uint16 port) {
    std::cout << "[Client] Connecting to " << host << " on port " << port << std::endl;

    ENetAddress address;
    ENetEvent event;

    enet_address_set_host(&address, host.c_str());
    address.port = port;

    peer = enet_host_connect(client, &address, 2, 0);
    if (!peer) {
        std::cerr << "[Client] No available peers for initiating connection.\n";
        exit(EXIT_FAILURE);
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "[Client] Connected to server.\n";
    } else {
        std::cerr << "[Client] Connection to server failed.\n";
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
                    std::cout << "[Client] Received Scene: " << payload << std::endl;
                    sceneReceivedCallback(payload);
                } else if (type == SEND_PLAYER_SPAWN) {
                    std::cout << "[Client] Received Player Spawn: " << payload << std::endl;

                    size_t delimiterPos = payload.find('|');
                    if (delimiterPos == std::string::npos) {
                        std::cerr << "[Client] Invalid payload format: " << payload << std::endl;
                        return;
                    }

                    std::string boolStr = payload.substr(0, delimiterPos);
                    std::string intStr = payload.substr(delimiterPos + 1);

                    bool isClient = (boolStr == "true");
                    int peerID = std::stoi(intStr);

                    if (spawnNewPlayerCallback) {
                        spawnNewPlayerCallback(isClient, peerID);
                    }
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

void Client::SetSpawnNewPlayerCallback(std::function<void(const bool isClient, const int peerID)> callback) {
    spawnNewPlayerCallback = callback;
}
