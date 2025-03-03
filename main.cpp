#include "src/Server.h"
#include <iostream>
int main(){
	int port = 6379;

	Server server(port);
	server.run();
	return 0;
}
