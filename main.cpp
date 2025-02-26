
int main(int argc, char** argv){
	int port = 6379;

	Server server(port);
	server.run();
	return 0;
}