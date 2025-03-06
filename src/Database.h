#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <deque>
#include <vector>
#include <fstream>
#include <mutex>


class Database {
public:
    Database();             
    ~Database();            

    void set(const std::string& key, const std::string& value);
    std::string get(const std::string& key) const;
    bool del(const std::string& key);

    
    long incr(const std::string& key);
    long decr(const std::string& key);

    
    bool expire(const std::string& key, int seconds);
    long ttl(const std::string& key);
    bool persist(const std::string& key);

    
    void mset(const std::vector<std::string>& keyValues);
    std::vector<std::string> mget(const std::vector<std::string>& keys);

    
    void lpush(const std::string& key, const std::string& value);
    void rpush(const std::string& key, const std::string& value);
    std::string lpop(const std::string& key);
    std::string rpop(const std::string& key);
    std::vector<std::string> lrange(const std::string& key, int start, int stop);

    
    std::string ping();

private:
    
    std::unordered_map<std::string, std::string> store_;
    std::unordered_map<std::string, std::chrono::system_clock::time_point> expirations_;
    std::unordered_map<std::string, std::deque<std::string>> lists_;

   
    std::ofstream aof_;  
    bool loading_;       

    
    mutable std::recursive_mutex dbMutex_;

 
    void persistCommand(const std::string& cmd);
    void loadAOF();

    
    bool isExpired(const std::string& key);
};
