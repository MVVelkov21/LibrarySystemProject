#pragma once
#include "precompiledHeader.h"

class mainMenu {
private:
	sqlite3* db;
public:
	const int screenWidth = 800;
	const int screenHeight = 450;
	void windowInit();
};
