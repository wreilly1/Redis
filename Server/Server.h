#pragma once

#include "Database.h"
#include "CommandParser.h"
#include <netinet/in.h>
#include <string>

class Server {
private:
	int port_;
	int serverFd;

	Database db_;
	CommandParser parser_;
	
	bool setupSocket();
	void acceptConnections();

public:
	Server(int port);
	~Server();

	void run();
};