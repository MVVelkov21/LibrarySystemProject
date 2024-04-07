#pragma once
#include "precompiledHeader.h"

class mainMenu {
private:
public:
	const int screenWidth = 800;
	const int screenHeight = 450;
	bool isUserExists(const string& username, sqlite3* db);
	void login(sqlite3* db);
	void windowInit();
};
