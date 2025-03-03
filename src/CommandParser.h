#pragma once

#include <string>
#include <vector>

class Database;

class CommandParser {
public:
    explicit CommandParser(Database& db);
    std::string parseAndExecute(const std::string& commandLine);

private:
    Database& db_;
    std::vector<std::string> split(const std::string& str, char delimiter);
};
