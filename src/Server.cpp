#include "Server.h"
#include "ThreadPool.h"   
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>


Server::Server(int port)
    : port_(port), serverFd_(-1), db_(), parser_(db_), pool_(4) 
{
}


Server::~Server() {
    if (serverFd_ != -1) {
        close(serverFd_);
    }
}

bool Server::setupSocket() {
    serverFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd_ < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    int opt = 1;
    if (setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options" << std::endl;
        return false;
    }

    sockaddr_in address;
    std::memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(serverFd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }

    if (listen(serverFd_, 5) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return false;
    }

    std::cout << "Server listening on port " << port_ << std::endl;
    return true;
}

// Accept incoming client connections and enqueue each connection handler to the thread pool.
void Server::acceptConnections() {
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(serverFd_, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientFd < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        // Enqueue a task for handling the client connection.
        pool_.enqueue([this, clientFd]() {
            char buffer[1024];
            while (true) {
                std::memset(buffer, 0, sizeof(buffer));
                ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
                if (bytesRead <= 0) {
                    // Client disconnected or error occurred.
                    close(clientFd);
                    break;
                }
                std::string commandLine(buffer);
                std::string response = parser_.parseAndExecute(commandLine);
                write(clientFd, response.c_str(), response.size());
                // If the client sends QUIT, the response will be "BYE\r\n"
                if (response == "BYE\r\n") {
                    close(clientFd);
                    break;
                }
            }
        });
    }
}

// Start the server: set up the socket and begin accepting connections.
void Server::run() {
    if (!setupSocket()) {
        std::cerr << "Could not setup server" << std::endl;
        return;
    }
    acceptConnections();
}
