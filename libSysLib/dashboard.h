#pragma once
#include "precompiledHeader.h"

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
	void insertBook(const string& isbn, const string& title, const string& author, const string& genre, const string& pubDate);
	void getBooksTaken(int* booksTaken, const char** genres, int numGenres);
	void viewDatabase();
	void addBook();
	void DrawInputBox(Rectangle textBox, char* inputText, int& letterCount, bool mouseOnText, int framesCounter);
public:
	int screenWidth = 800;
	int screenHeight = 450;

	void windowInit();
};