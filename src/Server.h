#pragma once

#include "Database.h"
#include "CommandParser.h"
#include <netinet/in.h>
#include <string>

class Server {
public:
    Server(int port);
    ~Server();

    void run();

private:
    int port_;
    int serverFd_;

    Database db_;
    CommandParser parser_;

    bool setupSocket();
    void acceptConnections();
};
