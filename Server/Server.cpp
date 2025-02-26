#include "Server.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

Server::Server(int port)
	: port_(port), serverFd_(-1), db_(), parser_(db_) {
}

Server::~Server() {
	if (serverFd_ != -1) {
		close(serverFd_);
	}
}

bool Server::setupSocket() {
	serverFd_ = socket(AF_INET, SOCK_STREAM, 0);
	if (serverFd_ < 0) {
		cerr << "Failed to create socket\n";
		return false;
	}

	int opt = 1;
	if (setsockopt(serverFd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		std::cerr << "Failed to set socket options\n";
		return false;
	}
	sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AD_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port_);

	if (bind(serverFd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
		cerr << "Failed to bind socket\n";
		return false;
	}

	if (listen(serverFd_, 5) < 0) {
		cerr << "Failed to listen on socket\n";
		return false;
	}

	cout << "Server listening on port " << port_ << std::endl;
	return true;
}

void Server::acceptConnections() {
	while (true) {
		sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int clientFd = accept(serverFd_, (struct sockaddr*)&clientAddr, &clientLen);
		if (clientFd < 0) {
			cerr << "Accept failed\n";
			continue;
		}
		char buffer[1024];
		while (true) {
			memset(buffer, 0, sizeof(buffer));
			ssize_t bytesRead = read(clientFd, buffer, sizeof(buffer) - 1);
			if (bytesRead <= 0) {
				// Client disconnected or error
				close(clientFd);
				break;
			}

			string commandLine(buffer);
			string response = parser_.parseAndExecute(commandLine);
			write(clientFd, response.c_str(), response.size());

		}
	}
}

void Server::run() {
	if (!setupSocket()) {
		cerr << "Could not setup server\n";
		return;
	}
	acceptConnections();
}