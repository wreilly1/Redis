#include "CommandParser.h"
#include "Database.h"
#include <sstream>

CommandParser::CommandPArser(Database& db) : db_(db) {}

string CommandParser::parseAndExecute(const string& commandLine) {
	auto tokens= split(commandLine, ' ');
	if (tokens.empty()){
		return "No Command Given";
	}

	string command = tokens[0];
	for (size_t i = 0; < i < command.size(); i++) {
		command[i] = static_cast<char> touper(static_cast<unisigned char>(command[i])));
	}

	if (command == "SET") {
		if (tokens.size() < 3) {
			return "Wrong number of arguments for SET \r\n";
		}
		db_.set(tokens[1], tokens[2]);
		retunr "Ok \r\n";
	}
	else if (command == "GET") {
		if (tokens.size() < 2) {
			return "Err wrong number of arguments for GET \r\n";
		}
		auto val = db_.get(tokens[1]);
		return val.empty() ? "(nil)\r\n" : (val + "\r\n");
	}
	else if (command == "DEL") {
		if (tokens.size() < 2) {
			return "Err wrong number of arguments for DEL \r\n";
		}
		bool deleted = db_.del(tokens[1]);
		return deleted ? "1\r\n" : "0\r\n";
	}
	return "Err unkown command\r\n"
	}

vector<string> CommandParser::split(const string& str, char delimiter) {
	vector<string> tokens;
	stringstream ss(str);
	string item;
	while (getline(ss, item, delimiter)) {
		if (!item.empty()) {
			tokens.push_back(item);
		}
	}
	return tokens;
}