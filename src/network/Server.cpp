#include "Server.h"

Server::Server(char* ipAddress, int port, Scene* sceneRef) : scene(sceneRef) {
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

            std::string receivedMsg;
            size_t separator = std::string::npos;

            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "Client connected.\n";
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    receivedMsg = std::string(reinterpret_cast<char*>(event.packet->data));
                    std::cout << "Packet received: " << receivedMsg << "\n";
                    
                    separator = receivedMsg.find('|');
                    if (separator != std::string::npos) {
                        int type = std::stoi(receivedMsg.substr(0, separator));
                        std::string payload = receivedMsg.substr(separator + 1);

                        if (type == REQUEST_SCENE) {
                            std::cout << "Scene request received. Sending scene.\n";
                            BroadcastScene(event.peer);
                        } else if (type == CHAT_MESSAGE) {
                            std::cout << "Message from client: " << payload << std::endl;
                        }
                    }

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

void Server::BroadcastScene(ENetPeer* peer) {
    std::string sceneData = scene->Serialize();
    std::string packetData = std::to_string(SEND_SCENE) + "|" + sceneData;

    ENetPacket* packet = enet_packet_create(packetData.c_str(), packetData.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}