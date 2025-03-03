#include "CommandParser.h"
#include "Database.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

CommandParser::CommandParser(Database& db) : db_(db) {}


// Helper function to trim whitespace (including spaces, tabs, \n, and \r) from both ends of a string.
std::string trim(const std::string& s) {
    // Find first character that is not a whitespace.
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";  // all whitespace

    // Find last character that is not a whitespace.
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

std::vector<std::string> CommandParser::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::string CommandParser::parseAndExecute(const std::string& commandLine) {
    std::string trimmedCommandLine = trim(commandLine);
    auto tokens = split(trimmedCommandLine, ' ');

    if (tokens.empty()) {
        return "ERR no command given\r\n";
    }

    // Convert command to uppercase
    std::string command = tokens[0];
    for (size_t i = 0; i < command.size(); ++i) {
        command[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(command[i])));
    }

    if (command == "SET") {
        if (tokens.size() < 3)
            return "ERR wrong number of arguments for 'SET'\r\n";
        db_.set(tokens[1], tokens[2]);
        return "OK\r\n";
    }
    else if (command == "GET") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'GET'\r\n";
        std::string val = db_.get(tokens[1]);
        return val.empty() ? "(nil)\r\n" : (val + "\r\n");
    }
    else if (command == "DEL") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'DEL'\r\n";
        bool deleted = db_.del(tokens[1]);
        return deleted ? "1\r\n" : "0\r\n";
    }
    else if (command == "INCR") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'INCR'\r\n";
        try {
            long newValue = db_.incr(tokens[1]);
            return std::to_string(newValue) + "\r\n";
        } catch (const std::runtime_error& e) {
            return std::string(e.what()) + "\r\n";
        }
    }
    else if (command == "DECR") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'DECR'\r\n";
        try {
            long newValue = db_.decr(tokens[1]);
            return std::to_string(newValue) + "\r\n";
        } catch (const std::runtime_error& e) {
            return std::string(e.what()) + "\r\n";
        }
    }
    else if (command == "EXPIRE") {
        if (tokens.size() < 3)
            return "ERR wrong number of arguments for 'EXPIRE'\r\n";
        int seconds = std::stoi(tokens[2]);
        bool success = db_.expire(tokens[1], seconds);
        return success ? "1\r\n" : "0\r\n";
    }
    else if (command == "TTL") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'TTL'\r\n";
        long ttlVal = db_.ttl(tokens[1]);
        return std::to_string(ttlVal) + "\r\n";
    }
    else if (command == "PERSIST") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'PERSIST'\r\n";
        bool success = db_.persist(tokens[1]);
        return success ? "1\r\n" : "0\r\n";
    }
    else if (command == "MSET") {
        if ((tokens.size() - 1) % 2 != 0)
            return "ERR wrong number of arguments for 'MSET'\r\n";
        // Create a vector from tokens[1] onward
        std::vector<std::string> keyValues(tokens.begin() + 1, tokens.end());
        db_.mset(keyValues);
        return "OK\r\n";
    }
    else if (command == "MGET") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'MGET'\r\n";
        std::vector<std::string> keys(tokens.begin() + 1, tokens.end());
        auto results = db_.mget(keys);
        std::string response;
        for (const auto& res : results) {
            response += (res.empty() ? "(nil)\r\n" : res + "\r\n");
        }
        return response;
    }
    else if (command == "LPUSH") {
        if (tokens.size() < 3)
            return "ERR wrong number of arguments for 'LPUSH'\r\n";
        db_.lpush(tokens[1], tokens[2]);
        return "OK\r\n";
    }
    else if (command == "RPUSH") {
        if (tokens.size() < 3)
            return "ERR wrong number of arguments for 'RPUSH'\r\n";
        db_.rpush(tokens[1], tokens[2]);
        return "OK\r\n";
    }
    else if (command == "LPOP") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'LPOP'\r\n";
        std::string val = db_.lpop(tokens[1]);
        return val.empty() ? "(nil)\r\n" : (val + "\r\n");
    }
    else if (command == "RPOP") {
        if (tokens.size() < 2)
            return "ERR wrong number of arguments for 'RPOP'\r\n";
        std::string val = db_.rpop(tokens[1]);
        return val.empty() ? "(nil)\r\n" : (val + "\r\n");
    }
    else if (command == "LRANGE") {
        if (tokens.size() < 4)
            return "ERR wrong number of arguments for 'LRANGE'\r\n";
        int start = std::stoi(tokens[2]);
        int stop = std::stoi(tokens[3]);
        auto list = db_.lrange(tokens[1], start, stop);
        std::string response;
        for (const auto& item : list) {
            response += item + "\r\n";
        }
        return response;
    }
    else if (command == "PING") {
        return db_.ping() + "\r\n";
    }
    else if (command == "QUIT") {
        return "BYE\r\n";
    }
    else if (command == "KILL") {
        std::cout << "Server shutdown initiated via KILL command." << std::endl;
        exit(0);
    }
    return "ERR unknown command\r\n";
}
