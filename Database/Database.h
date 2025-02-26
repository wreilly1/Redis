#pragma once
#include <string>
#include <unordered_map>
#include <mutex> // if multi-threaded

using namespace std;

Class Database{

private:
	unordered_map<string,string> store_;

public:

	void set(const string key, const string& value);
	string get(const string& key) const;
	bool del(const string& key);
}