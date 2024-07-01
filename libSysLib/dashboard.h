#pragma once
#include "book.h"

class dashboard {
private:
	Font customFont;
	
	int numGenres;
	int* booksTaken;
	float maxBooks = 20.0f;

	int framesCounter = 0;
	const int animationFrames = 30;

	bool showDropdown = false;
	float dropdownHeight = 0.0f;
	const float maxDropdownHeight = 130.0f;
	const float dropdownSpeed = 5.0f;

	void createDatabase();
	void getBooksTaken(int* booksTaken, const char** genres, int numGenres);
	void viewDatabase();
	int getTotalBooks();
	int getTakenBooks();
	
public:
	int screenWidth = 800;
	int screenHeight = 450;

	void windowInit();
};