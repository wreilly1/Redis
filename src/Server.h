#pragma once

#include "Database.h"
#include "CommandParser.h"
#include "ThreadPool.h"  
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
    ThreadPool pool_;

    bool setupSocket();
    void acceptConnections();
};
