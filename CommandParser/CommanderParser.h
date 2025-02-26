#pragma once
#include <string>
#include <vector>

using namespace std;

class Database;

Class CommandParser{
public:
string parseAndExecute(const string& commandLine);

private:
	Database& db_;
	vector<string> split(const string& str, char delimiter);

}