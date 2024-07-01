#include "../libSysLib/book.h"

void book::addBook() {
    char isbn[MAX_INPUT_CHARS + 1] = "\0";
    int isbnCount = 0;

    char title[MAX_INPUT_CHARS + 1] = "\0";
    int titleCount = 0;

    char author[MAX_INPUT_CHARS + 1] = "\0";
    int authorCount = 0;

    char genre[MAX_INPUT_CHARS + 1] = "\0";
    int genreCount = 0;

    char pubDate[MAX_INPUT_CHARS + 1] = "\0";
    int pubDateCount = 0;

    Rectangle isbnBox = { 400, 100, 225, 30 };
    Rectangle titleBox = { 400, 150, 225, 30 };
    Rectangle authorBox = { 400, 200, 225, 30 };
    Rectangle genreBox = { 400, 250, 225, 30 };
    Rectangle pubDateBox = { 400, 300, 225, 30 };

    Rectangle addButton = { 400, 350, 100, 30 };
    Rectangle cancelButton = { 525, 350, 100, 30 };

    bool mouseOnIsbn = false;
    bool mouseOnTitle = false;
    bool mouseOnAuthor = false;
    bool mouseOnGenre = false;
    bool mouseOnPubDate = false;

    int framesCounter = 0;

    while (!WindowShouldClose()) {
        framesCounter++;

        mouseOnIsbn = CheckCollisionPointRec(GetMousePosition(), isbnBox);
        mouseOnTitle = CheckCollisionPointRec(GetMousePosition(), titleBox);
        mouseOnAuthor = CheckCollisionPointRec(GetMousePosition(), authorBox);
        mouseOnGenre = CheckCollisionPointRec(GetMousePosition(), genreBox);
        mouseOnPubDate = CheckCollisionPointRec(GetMousePosition(), pubDateBox);

        if (mouseOnIsbn || mouseOnTitle || mouseOnAuthor || mouseOnGenre || mouseOnPubDate) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125)) {
                    if (mouseOnIsbn && isbnCount < MAX_INPUT_CHARS) {
                        isbn[isbnCount] = (char)key;
                        isbn[isbnCount + 1] = '\0';
                        isbnCount++;
                    }
                    else if (mouseOnTitle && titleCount < MAX_INPUT_CHARS) {
                        title[titleCount] = (char)key;
                        title[titleCount + 1] = '\0';
                        titleCount++;
                    }
                    else if (mouseOnAuthor && authorCount < MAX_INPUT_CHARS) {
                        author[authorCount] = (char)key;
                        author[authorCount + 1] = '\0';
                        authorCount++;
                    }
                    else if (mouseOnGenre && genreCount < MAX_INPUT_CHARS) {
                        genre[genreCount] = (char)key;
                        genre[genreCount + 1] = '\0';
                        genreCount++;
                    }
                    else if (mouseOnPubDate && pubDateCount < MAX_INPUT_CHARS) {
                        pubDate[pubDateCount] = (char)key;
                        pubDate[pubDateCount + 1] = '\0';
                        pubDateCount++;
                    }
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (mouseOnIsbn && isbnCount > 0) {
                    isbnCount--;
                    isbn[isbnCount] = '\0';
                }
                else if (mouseOnTitle && titleCount > 0) {
                    titleCount--;
                    title[titleCount] = '\0';
                }
                else if (mouseOnAuthor && authorCount > 0) {
                    authorCount--;
                    author[authorCount] = '\0';
                }
                else if (mouseOnGenre && genreCount > 0) {
                    genreCount--;
                    genre[genreCount] = '\0';
                }
                else if (mouseOnPubDate && pubDateCount > 0) {
                    pubDateCount--;
                    pubDate[pubDateCount] = '\0';
                }
            }
        }
        else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextEx(customFont, "Add Book", { (800 - MeasureTextEx(customFont, "Add Book", 80, -3).x) / 2, 20 }, 80, -3, GREEN);

        DrawTextEx(customFont, "ISBN:", { 300, 100 }, 20, -2, GREEN);
        DrawTextEx(customFont, "Title:", { 300, 150 }, 20, -2, GREEN);
        DrawTextEx(customFont, "Author:", { 300, 200 }, 20, -2, GREEN);
        DrawTextEx(customFont, "Genre:", { 300, 250 }, 20, -2, GREEN);
        DrawTextEx(customFont, "Publication Date:", { 300, 300 }, 20, -2, GREEN);

        DrawInputBox(isbnBox, isbn, isbnCount, mouseOnIsbn, framesCounter);
        DrawInputBox(titleBox, title, titleCount, mouseOnTitle, framesCounter);
        DrawInputBox(authorBox, author, authorCount, mouseOnAuthor, framesCounter);
        DrawInputBox(genreBox, genre, genreCount, mouseOnGenre, framesCounter);
        DrawInputBox(pubDateBox, pubDate, pubDateCount, mouseOnPubDate, framesCounter);

        DrawRectangleRec(addButton, GREEN);
        DrawTextEx(customFont, "Add", { addButton.x + 10, addButton.y + 5 }, 20, -2, BLACK);

        DrawRectangleRec(cancelButton, RED);
        DrawTextEx(customFont, "Cancel", { cancelButton.x + 10, cancelButton.y + 5 }, 20, -2, BLACK);

        if (CheckCollisionPointRec(GetMousePosition(), addButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            insertBook(isbn, title, author, genre, pubDate);
            break;
        }
        if (CheckCollisionPointRec(GetMousePosition(), cancelButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            break;
        }

        EndDrawing();
    }
}


void book::deleteBook() {
    char title[MAX_INPUT_CHARS + 1] = "\0";
    int titleCount = 0;

    Rectangle titleBox = { 300, 150, 225, 30 };
    Rectangle searchButton = { 300, 200, 100, 30 };
    Rectangle cancelButton = { 425, 200, 100, 30 };

    bool mouseOnTitle = false;
    int framesCounter = 0;

    while (!WindowShouldClose()) {
        framesCounter++;

        mouseOnTitle = CheckCollisionPointRec(GetMousePosition(), titleBox);

        if (mouseOnTitle) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125)) {
                    if (titleCount < MAX_INPUT_CHARS) {
                        title[titleCount] = (char)key;
                        title[titleCount + 1] = '\0';
                        titleCount++;
                    }
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (titleCount > 0) title[--titleCount] = '\0';
            }
        }
        else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }

        if (CheckCollisionPointRec(GetMousePosition(), searchButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            sqlite3* DB;
            sqlite3_stmt* stmt;

            int exit = sqlite3_open("books.db", &DB);
            if (exit) {
                cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
                break;
            }

            string sql = "SELECT * FROM BOOKS WHERE TITLE LIKE '%" + string(title) + "%';";
            sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);

            vector<string> results;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmt, 0);
                const unsigned char* bookTitle = sqlite3_column_text(stmt, 2);
                string result = to_string(id) + " | " + string(reinterpret_cast<const char*>(bookTitle));
                results.push_back(result);
            }

            sqlite3_finalize(stmt);
            sqlite3_close(DB);

            bool selectingBook = true;
            int selectedBook = -1;

            while (selectingBook && !WindowShouldClose()) {
                BeginDrawing();
                ClearBackground(BLACK);

                DrawTextEx(customFont, "Select a Book to Delete", { (float)screenWidth / 2 - MeasureText("Select a Book to Delete", 40) / 2, 40 }, 40, 2, GREEN);

                int yOffset = 100;
                for (size_t i = 0; i < results.size(); ++i) {
                    DrawTextEx(customFont, results[i].c_str(), { 300, static_cast<float>(yOffset) }, 20, 2, GREEN);
                    yOffset += 30;
                }

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mousePos = GetMousePosition();
                    int index = (mousePos.y - 100) / 30;
                    if (index >= 0 && index < results.size()) {
                        selectedBook = index;
                        selectingBook = false;
                    }
                }

                EndDrawing();
            }

            if (selectedBook != -1) {
                string selected = results[selectedBook];
                int id = stoi(selected.substr(0, selected.find(" | ")));

                exit = sqlite3_open("books.db", &DB);
                if (exit) {
                    cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
                    break;
                }

                sql = "DELETE FROM BOOKS WHERE ID = " + to_string(id) + ";";
                exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, 0);

                if (exit != SQLITE_OK) {
                    cerr << "Error Deleting Data: " << sqlite3_errmsg(DB) << endl;
                }
                else {
                    cout << "Book deleted successfully" << endl;
                }

                sqlite3_close(DB);
                break;
            }
        }

        if (CheckCollisionPointRec(GetMousePosition(), cancelButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextEx(customFont, "Delete Book", { 300, 40 }, 40, 2, GREEN);

        DrawTextEx(customFont, "Title:", { 220, 158 }, 20, 2, GREEN);
        DrawInputBox(titleBox, title, titleCount, mouseOnTitle, framesCounter);

        DrawRectangleRec(searchButton, DARKGRAY);
        DrawTextEx(customFont, "Search", { 310, 205 }, 20, 2, GREEN);

        DrawRectangleRec(cancelButton, DARKGRAY);
        DrawTextEx(customFont, "Cancel", { 440, 205 }, 20, 2, GREEN);

        EndDrawing();
    }
}

void book::insertBook(const string& isbn, const string& title, const string& author, const string& genre, const string& pubDate) {
    sqlite3* DB;
    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    string sql = "INSERT INTO BOOKS (ISBN, TITLE, AUTHOR, GENRE, PUBLICATION_DATE) VALUES ('" +
        isbn + "', '" + title + "', '" + author + "', '" + genre + "', '" + pubDate + "');";

    char* messageError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error Inserting Data: " << messageError << endl;
        sqlite3_free(messageError);
    }
    else {
        cout << "Book inserted successfully" << endl;
    }

    sqlite3_close(DB);
}

void book::generateBooks() {
    sqlite3* DB;
    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    const char* genres[] = { "Fiction", "Non-Fiction", "Science", "History", "Art" };
    int numGenres = sizeof(genres) / sizeof(genres[0]);

    srand(time(0));

    for (int i = 0; i < 100; i++) {
        string isbn = to_string(rand() % 1000000000 + 100000000);
        string title = "Book " + to_string(i + 1);
        string author = "Author " + to_string(rand() % 10 + 1);
        string genre = genres[rand() % numGenres];
        string pubDate = to_string(rand() % 2022 + 1900) + "-" + to_string(rand() % 12 + 1) + "-" + to_string(rand() % 28 + 1);
        string status = (rand() % 2 == 0) ? "FREE" : "TAKEN";

        string sql = "INSERT INTO BOOKS (ISBN, TITLE, AUTHOR, GENRE, PUBLICATION_DATE, STATUS) VALUES ('" +
            isbn + "', '" + title + "', '" + author + "', '" + genre + "', '" + pubDate + "', '" + status + "');";

        char* messageError;
        exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

        if (exit != SQLITE_OK) {
            cerr << "Error inserting data: " << messageError << endl;
            sqlite3_free(messageError);
        }
    }

    sqlite3_close(DB);
}

void book::DrawInputBox(Rectangle textBox, char* inputText, int& letterCount, bool mouseOnText, int framesCounter) {
    DrawRectangleRec(textBox, DARKGRAY);
    if (mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, GREEN);
    else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

    DrawText(inputText, (int)textBox.x + 5, (int)textBox.y + 8, 20, GREEN);

    if (mouseOnText) {
        if (letterCount < MAX_INPUT_CHARS) {
            if (((framesCounter / 20) % 2) == 0) DrawText("_", (int)textBox.x + 8 + MeasureText(inputText, 20), (int)textBox.y + 12, 20, GREEN);
        }
    }
}