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
    const int TICK_RATE = 60;
    const int MS_PER_TICK = 1000 / TICK_RATE;

    ENetEvent event;
    auto nextTick = std::chrono::high_resolution_clock::now();

    while (true) {
        // Constant Interval
        while (enet_host_service(server, &event, 0) > 0) {
            HandleEvent(event);
        }

        // Fixed Interval
        auto now = std::chrono::high_resolution_clock::now();
        if (now >= nextTick) {
            // Put update logic here
            // UpdateGameState();

            nextTick += std::chrono::milliseconds(MS_PER_TICK);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void Server::HandleEvent(ENetEvent& event) {
    std::string receivedMsg;
    size_t separator = std::string::npos;

    switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout << "[Server] Client connected.\n";
            break;

        case ENET_EVENT_TYPE_RECEIVE:
            receivedMsg = std::string(reinterpret_cast<char*>(event.packet->data));
            //std::cout << "[Server] Packet received: " << receivedMsg << "\n";
            
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
                } else if (type == SEND_PLAYER_INPUT) {
                    //std::cout << "[Server] Player input received: " << payload << std::endl;
                    // get peer id from &server->peers. compare it to received peer id
                    int peerID = getPeerID(event.peer);
                    scene->HandlePlayerInput(peerID, payload);
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

void Server::BroadcastScene(ENetPeer* peer) {
    std::string sceneData = scene->Serialize();
    std::string packetData = std::to_string(SEND_SCENE) + "|" + sceneData;

    ENetPacket* packet = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

void Server::BroadcastPlayerSpawn(ENetPeer* peer) {
    // Find peer ID (index of the requesting peer)
    int peerID = getPeerID(peer);

    // Send "true|peerID" to the requesting peer
    std::string packetData = std::to_string(SEND_PLAYER_SPAWN) + "|true|" + std::to_string(peerID);
    ENetPacket* packet = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);

    // Send "false|peerID" to all other connected peers
    packetData = std::to_string(SEND_PLAYER_SPAWN) + "|false|" + std::to_string(peerID);
    
    for (size_t i = 0; i < server->peerCount; ++i) {
        ENetPeer* otherPeer = &server->peers[i];
        
        if (otherPeer->state == ENET_PEER_STATE_CONNECTED && otherPeer != peer) {
            ENetPacket* otherPacket = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(otherPeer, 0, otherPacket);
        }
    }
}

int Server::getPeerID(ENetPeer* peer) {
    int peerID = -1;
    for (size_t i = 0; i < server->peerCount; ++i) {
        if (&server->peers[i] == peer) {
            peerID = static_cast<int>(i);
            break;
        }
    }

    if (peerID == -1) {
        std::cerr << "[Server] Error: Requesting peer not found in server->peers!" << std::endl;
        return -1;
    }

    return peerID;
}
