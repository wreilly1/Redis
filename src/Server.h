#pragma once

#include "Database.h"
#include "CommandParser.h"
#include "ThreadPool.h"  // Include your ThreadPool header
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

    // Add a thread pool member for handling client connections.
    ThreadPool pool_;

    bool setupSocket();
    void acceptConnections();
};
