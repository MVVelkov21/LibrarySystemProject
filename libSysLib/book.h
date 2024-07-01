#pragma once
#include "precompiledHeader.h"

#define MAX_INPUT_CHARS 100

class book {
private:
	Font customFont;
	int screenWidth;
	int screenHeight;
public:
	book(Font custom, int width, int height) {
		this->customFont = custom;
		this->screenWidth = width;
		this->screenHeight = height;
	}

	void generateBooks();
	void deleteBook();
	void addBook();
	void insertBook(const string& isbn, const string& title, const string& author, const string& genre, const string& pubDate);
	void DrawInputBox(Rectangle textBox, char* inputText, int& letterCount, bool mouseOnText, int framesCounter);
};