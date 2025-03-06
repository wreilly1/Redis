#include "Database.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <cstdlib>

Database::Database() : loading_(false) {
    // Open the AOF file in append mode. It will be created if it doesn't exist.
    aof_.open("appendonly.aof", std::ios::app);
    if (!aof_.is_open()) {
        std::cerr << "Warning: Could not open appendonly.aof for persistence." << std::endl;
    }
    // Load persisted commands from the AOF file.
    loadAOF();
}

Database::~Database() {
    if (aof_.is_open()) {
        aof_.close();
    }
}

void Database::persistCommand(const std::string& cmd) {
    if (!loading_ && aof_.is_open()) {
        aof_ << cmd << "\n";
        aof_.flush();
    }
}

void Database::loadAOF() {
    std::ifstream infile("appendonly.aof");
    if (!infile.is_open()) {
        std::cout << "No AOF file found. Starting with an empty database." << std::endl;
        return;
    }
    std::string line;
    loading_ = true; // Disable logging while loading
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        if (command == "SET") {
            std::string key, value;
            iss >> key >> value;
            set(key, value); // These calls won't re-log because loading_ is true.
        } else if (command == "DEL") {
            std::string key;
            iss >> key;
            del(key);
        } else if (command == "EXPIRE") {
            std::string key;
            int seconds;
            iss >> key >> seconds;
            expire(key, seconds);
        } else if (command == "PERSIST") {
            std::string key;
            iss >> key;
            persist(key);
        } else if (command == "LPUSH") {
            std::string key, value;
            iss >> key >> value;
            lpush(key, value);
        } else if (command == "RPUSH") {
            std::string key, value;
            iss >> key >> value;
            rpush(key, value);
        } else if (command == "LPOP") {
            std::string key;
            iss >> key;
            lpop(key);
        } else if (command == "RPOP") {
            std::string key;
            iss >> key;
            rpop(key);
        }
        // Extend this parser for other commands as needed.
    }
    loading_ = false;
    std::cout << "AOF loaded." << std::endl;
}

void Database::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    std::cout << "SET: " << key << " -> " << value << std::endl;
    store_[key] = value;
    persistCommand("SET " + key + " " + value);
}

std::string Database::get(const std::string& key) const {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    auto it = store_.find(key);
    if (it != store_.end()) {
        return it->second;
    }
    return "";
}

bool Database::del(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    bool existed = store_.erase(key) > 0;
    expirations_.erase(key);
    lists_.erase(key);
    if (existed) {
        persistCommand("DEL " + key);
    }
    return existed;
}

long Database::incr(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    std::string val = get(key);
    long num = 0;
    if (!val.empty()) {
        char* endPtr = nullptr;
        num = std::strtol(val.c_str(), &endPtr, 10);
        if (*endPtr != '\0') {
            throw std::runtime_error("ERR value is not an integer");
        }
    }
    num++;
    set(key, std::to_string(num));  // set() will persist.
    return num;
}

long Database::decr(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    std::string val = get(key);
    long num = 0;
    if (!val.empty()) {
        char* endPtr = nullptr;
        num = std::strtol(val.c_str(), &endPtr, 10);
        if (*endPtr != '\0') {
            throw std::runtime_error("ERR value is not an integer");
        }
    }
    num--;
    set(key, std::to_string(num));
    return num;
}


bool Database::expire(const std::string& key, int seconds) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    std::cout << "EXPIRE: " << key << " for " << seconds << " seconds" << std::endl;
    if (store_.find(key) == store_.end()) {
        return false;
    }
    auto now = std::chrono::system_clock::now();
    expirations_[key] = now + std::chrono::seconds(seconds);
    persistCommand("EXPIRE " + key + " " + std::to_string(seconds));
    return true;
}

long Database::ttl(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    if (store_.find(key) == store_.end()) {
        return -2; // key does not exist
    }
    if (expirations_.find(key) == expirations_.end()) {
        return -1; // no expiration set
    }
    auto now = std::chrono::system_clock::now();
    auto expTime = expirations_[key];
    auto diff = std::chrono::duration_cast<std::chrono::seconds>(expTime - now).count();
    if (diff <= 0) {
        del(key);
        return -2;
    }
    return diff;
}

bool Database::persist(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    if (expirations_.erase(key) > 0) {
        persistCommand("PERSIST " + key);
        return true;
    }
    return false;
}

void Database::mset(const std::vector<std::string>& keyValues) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    for (size_t i = 0; i < keyValues.size(); i += 2) {
        set(keyValues[i], keyValues[i + 1]);
    }
}

std::vector<std::string> Database::mget(const std::vector<std::string>& keys) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    std::vector<std::string> results;
    for (const auto& key : keys) {
        results.push_back(get(key));
    }
    return results;
}

void Database::lpush(const std::string& key, const std::string& value) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    lists_[key].push_front(value);
    persistCommand("LPUSH " + key + " " + value);
}

void Database::rpush(const std::string& key, const std::string& value) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    lists_[key].push_back(value);
    persistCommand("RPUSH " + key + " " + value);
}

std::string Database::lpop(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    if (lists_.find(key) == lists_.end() || lists_[key].empty())
        return "";
    std::string val = lists_[key].front();
    lists_[key].pop_front();
    persistCommand("LPOP " + key);
    return val;
}

std::string Database::rpop(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    if (lists_.find(key) == lists_.end() || lists_[key].empty())
        return "";
    std::string val = lists_[key].back();
    lists_[key].pop_back();
    persistCommand("RPOP " + key);
    return val;
}

std::vector<std::string> Database::lrange(const std::string& key, int start, int stop) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    std::vector<std::string> result;
    if (lists_.find(key) == lists_.end()) {
        return result;
    }
    auto& dq = lists_[key];
    int size = dq.size();
    if (start < 0) start = size + start;
    if (stop < 0) stop = size + stop;
    if (start < 0) start = 0;
    if (stop >= size) stop = size - 1;
    if (start > stop) return result;
    for (int i = start; i <= stop; ++i) {
        result.push_back(dq[i]);
    }
    return result;
}

std::string Database::ping() {
    return "PONG";
}

bool Database::isExpired(const std::string& key) {
    std::lock_guard<std::recursive_mutex> lock(dbMutex_);
    auto it = expirations_.find(key);
    if (it == expirations_.end()) {
        return false;
    }
    auto now = std::chrono::system_clock::now();
    if (now >= it->second) {
        del(key); 
        return true;
    }
    return false;
}
