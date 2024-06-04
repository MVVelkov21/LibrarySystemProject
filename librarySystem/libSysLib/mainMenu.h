#pragma once
#include "precompiledHeader.h"

class mainMenu {
private:
	int screenWidth = 800;
	int screenHeight = 450;
	int choice = 0;
	sqlite3* db;
	bool isUserExists(const string& username, sqlite3* db);
	bool checkUser(sqlite3* db);
public:
	void saveLoginInfo(const string& username, const string& password, sqlite3* db);
	void windowInit();
};
