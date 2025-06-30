#include "Server.h"

Server::Server(char* ipAddress, int port, Scene* sceneRef) : scene(sceneRef) {
    if (enet_initialize() != 0) {
        std::cerr << "[Server] ENet initialization failed.\n";
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
        std::cerr << "[Server] Failed to create ENet server.\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "[Server] Server started on port " << port << std::endl;
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

            std::string receivedMsg;
            size_t separator = std::string::npos;

            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "[Server] Client connected.\n";
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    receivedMsg = std::string(reinterpret_cast<char*>(event.packet->data));
                    std::cout << "[Server] Packet received: " << receivedMsg << "\n";
                    
                    separator = receivedMsg.find('|');
                    if (separator != std::string::npos) {
                        int type = std::stoi(receivedMsg.substr(0, separator));
                        std::string payload = receivedMsg.substr(separator + 1);

                        if (type == REQUEST_SCENE) {
                            std::cout << "[Server] Scene request received. Sending scene.\n";
                            BroadcastScene(event.peer);
                        } else if (type == CHAT_MESSAGE) {
                            std::cout << "[Server] Message from client: " << payload << std::endl;
                        } else if (type == REQUEST_PLAYER_SPAWN) {
                            std::cout << "[Server] Player spawn request received. Sending player spawn.\n";
                            BroadcastPlayerSpawn(event.peer);
                        }
                    }

                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "[Server] Client disconnected.\n";
                    break;

                default:
                    break;
            }
        }
    }
}

void Server::BroadcastScene(ENetPeer* peer) {
    std::string sceneData = scene->Serialize();
    std::string packetData = std::to_string(SEND_SCENE) + "|" + sceneData;

    ENetPacket* packet = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void Server::BroadcastPlayerSpawn(ENetPeer* peer) {
    // Send "true" to the requesting peer
    std::string packetData = std::to_string(SEND_PLAYER_SPAWN) + "|true";
    ENetPacket* packet = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);

    // Send "false" to all other peers
    packetData = std::to_string(SEND_PLAYER_SPAWN) + "|false";
    
    for (size_t i = 0; i < server->peerCount; ++i) {
        ENetPeer* otherPeer = &server->peers[i];
        
        // Only send to connected peers that are not the requesting peer
        if (otherPeer->state == ENET_PEER_STATE_CONNECTED && otherPeer != peer) {
            ENetPacket* otherPacket = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(otherPeer, 0, otherPacket);
        }
    }
}
