#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <deque>
#include <vector>
#include <fstream>
#include <mutex>

// The Database class implements a mini‑Redis in‑memory datastore with AOF persistence.
// Every write command is logged to an append‑only file ("appendonly.aof") which is replayed
// on startup to reconstruct the state.
class Database {
public:
    Database();             // Constructor: opens the AOF file and loads persisted commands.
    ~Database();            // Destructor: closes the AOF file.

    // Core commands
    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
    bool del(const std::string& key);

    // Numeric commands
    long incr(const std::string& key);
    long decr(const std::string& key);

    // Expiration commands
    bool expire(const std::string& key, int seconds);
    long ttl(const std::string& key);
    bool persist(const std::string& key);

    // Multiple key operations
    void mset(const std::vector<std::string>& keyValues);
    std::vector<std::string> mget(const std::vector<std::string>& keys);

    // List commands
    void lpush(const std::string& key, const std::string& value);
    void rpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    std::string rpop(const std::string& key);
    std::vector<std::string> lrange(const std::string& key, int start, int stop);

    // Server/Connection command
    std::string ping();

private:
    // In‑memory data structures.
    std::unordered_map<std::string, std::string> store_;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> expirations_;
    std::unordered_map<std::string, std::deque<std::string>> lists_;

    // Persistence members for AOF:
    std::ofstream aof_;  // Append‑only file stream.
    bool loading_;       // Flag to disable persistence while loading AOF.

    // Mutex to protect access to shared data (using a recursive mutex for nested locking).
    mutable std::recursive_mutex dbMutex_;

    // Persistence helper functions.
    void persistCommand(const std::string& cmd);
    void loadAOF();

    // Helper: Checks if key is expired (and removes it if so).
    bool isExpired(const std::string& key);
};
