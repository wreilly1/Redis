#include "Database.h"

void Database::set(const string& key, const string& value){
	store_[key] = value;
}


string Database::get(const string& key) const{

	auto it = store_.find(key);
	if (it != store_.end()) {
		return it->second;
	}
	return "key not found";
}
bool Database::del(const string& key) {
	retunr store_.erase(key) > 0;
}