#include "../libSysLib/dashboard.h"

void dashboard::createDatabase() {
    sqlite3* DB;
    int exit = 0;
    exit = sqlite3_open("books.db", &DB);

    string sql = "CREATE TABLE IF NOT EXISTS BOOKS("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "ISBN TEXT NOT NULL, "
        "TITLE TEXT NOT NULL, "
        "AUTHOR TEXT NOT NULL, "
        "GENRE TEXT NOT NULL, "
        "PUBLICATION_DATE TEXT NOT NULL, "
        "STATUS TEXT NOT NULL DEFAULT 'FREE');";

    char* messageError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error Creating Table: " << messageError << endl;
        sqlite3_free(messageError);
    }
    else {
        cout << "Table created successfully" << endl;
    }
    sqlite3_close(DB);
}

void dashboard::insertBook(const string& isbn, const string& title, const string& author, const string& genre, const string& pubDate) {
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

void dashboard::getBooksTaken(int* booksTaken, const char** genres, int numGenres) {
    sqlite3* DB;
    sqlite3_stmt* stmt;

    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    for (int i = 0; i < numGenres; ++i) {
        string sql = "SELECT COUNT(*) FROM BOOKS WHERE GENRE = '" + string(genres[i]) + "';";
        sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            booksTaken[i] = sqlite3_column_int(stmt, 0);
        }
        else {
            booksTaken[i] = 0;
        }

        sqlite3_finalize(stmt);
    }

    sqlite3_close(DB);
}

void dashboard::viewDatabase() {
    sqlite3* DB;
    sqlite3_stmt* stmt;

    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return;
    }    

    string sql = "SELECT * FROM BOOKS;";
    sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);

    const int maxRecords = 1000;
    vector<pair<string, string>> records; // pair of record string and status
    while (sqlite3_step(stmt) == SQLITE_ROW && records.size() < maxRecords) {
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char* isbn = sqlite3_column_text(stmt, 1);
        const unsigned char* title = sqlite3_column_text(stmt, 2);
        const unsigned char* author = sqlite3_column_text(stmt, 3);
        const unsigned char* genre = sqlite3_column_text(stmt, 4);
        const unsigned char* pubDate = sqlite3_column_text(stmt, 5);
        const unsigned char* status = sqlite3_column_text(stmt, 6);

        string record = to_string(id) + " | " +
            string(reinterpret_cast<const char*>(isbn)) + " | " +
            string(reinterpret_cast<const char*>(title)) + " | " +
            string(reinterpret_cast<const char*>(author)) + " | " +
            string(reinterpret_cast<const char*>(genre)) + " | " +
            string(reinterpret_cast<const char*>(pubDate));

        records.push_back(make_pair(record, string(reinterpret_cast<const char*>(status))));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);

    bool viewingDatabase = true;
    int selectedIndex = 0; // Index of the currently selected record
    int scrollOffset = 0;
    const int maxVisibleLines = 20;
    const int lineHeight = 20;
    const int tableWidth = 800;
    const Vector2 mousePos = GetMousePosition();
    const float boxPadding = 2.0f; // Padding around the selected record

    while (viewingDatabase && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        int startIndex = scrollOffset;
        int endIndex = min(scrollOffset + maxVisibleLines, static_cast<int>(records.size()));

        int yOffset = 20;
        for (int i = startIndex; i < endIndex; ++i) {
            // Calculate text bounds for collision detection
            Vector2 textSize = MeasureTextEx(customFont, records[i].first.c_str(), 20, 1);
            Rectangle textBounds = { 20, static_cast<float>(yOffset), textSize.x, lineHeight };

            // Check if mouse is hovering over this record
            if (CheckCollisionPointRec(mousePos, textBounds)) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Update database with new status for the selected book
                    int bookId = i + 1; // SQL IDs start from 1
                    string currentStatus = records[i].second;
                    string newStatus = (currentStatus == "FREE") ? "TAKEN" : "FREE";

                    if (sqlite3_open("books.db", &DB) == SQLITE_OK) {
                        string updateSql = "UPDATE BOOKS SET STATUS = '" + newStatus + "' WHERE ID = " + to_string(bookId) + ";";
                        char* errMsg;
                        if (sqlite3_exec(DB, updateSql.c_str(), NULL, 0, &errMsg) == SQLITE_OK) {
                            cout << "Book status updated successfully" << endl;
                            // Update local records
                            records[i].second = newStatus;
                        }
                        else {
                            cerr << "Error updating book status: " << errMsg << endl;
                            sqlite3_free(errMsg);
                        }
                        sqlite3_close(DB);
                    }
                    else {
                        cerr << "Can't open database for update" << endl;
                    }
                }
            }

            // Draw text with custom font
            Color textColor = (records[i].second == "TAKEN") ? RED : GREEN;
            DrawTextEx(customFont, records[i].first.c_str(), { 20, static_cast<float>(yOffset) }, 20, 1, textColor);

            yOffset += lineHeight;
        }

        // Handle mouse scroll wheel to navigate records and change selection
        int scroll = GetMouseWheelMove();
        if (scroll != 0) {
            // Invert scroll direction
            selectedIndex -= scroll;
            if (selectedIndex < 0) {
                selectedIndex = 0;
            }
            else if (selectedIndex >= static_cast<int>(records.size())) {
                selectedIndex = static_cast<int>(records.size()) - 1;
            }

            // Adjust scrollOffset to keep selectedIndex within visible range
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            }
            else if (selectedIndex >= scrollOffset + maxVisibleLines) {
                scrollOffset = selectedIndex - maxVisibleLines + 1;
            }
        }

        // Draw box around selected record
        if (selectedIndex >= startIndex && selectedIndex < endIndex) {
            Vector2 selectedTextSize = MeasureTextEx(customFont, records[selectedIndex].first.c_str(), 20, 1);
            Rectangle selectedTextBounds = { 20, static_cast<float>(20 + (selectedIndex - startIndex) * lineHeight), selectedTextSize.x, lineHeight };

            // Draw box around selected record
            DrawRectangleLinesEx({ selectedTextBounds.x - boxPadding, selectedTextBounds.y - boxPadding, selectedTextBounds.width + 2 * boxPadding, selectedTextBounds.height + 2 * boxPadding }, 2, GRAY);
        }

        if (IsKeyPressed(KEY_ENTER)) {
            // Update database with new status for the selected book
            int bookId = selectedIndex + 1; // SQL IDs start from 1
            string currentStatus = records[selectedIndex].second;
            string newStatus = (currentStatus == "FREE") ? "TAKEN" : "FREE";

            if (sqlite3_open("books.db", &DB) == SQLITE_OK) {
                string updateSql = "UPDATE BOOKS SET STATUS = '" + newStatus + "' WHERE ID = " + to_string(bookId) + ";";
                char* errMsg;
                if (sqlite3_exec(DB, updateSql.c_str(), NULL, 0, &errMsg) == SQLITE_OK) {
                    cout << "Book status updated successfully" << endl;
                    // Update local records
                    records[selectedIndex].second = newStatus;
                }
                else {
                    cerr << "Error updating book status: " << errMsg << endl;
                    sqlite3_free(errMsg);
                }
                sqlite3_close(DB);
            }
            else {
                cerr << "Can't open database for update" << endl;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) {
            viewingDatabase = false;
        }

        EndDrawing();
    }   
}


#define MAX_INPUT_CHARS 100

void dashboard::DrawInputBox(Rectangle textBox, char* inputText, int& letterCount, bool mouseOnText, int framesCounter) {
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

void dashboard::addBook() {    
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

void dashboard::takeBook(int id) {
    sqlite3* DB;
    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    string sql = "UPDATE BOOKS SET STATUS = 'TAKEN' WHERE ID = " + to_string(id) + ";";

    char* messageError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error updating book status: " << messageError << endl;
        sqlite3_free(messageError);
    }
    else {
        cout << "Book taken successfully" << endl;
    }

    sqlite3_close(DB);
}

void dashboard::returnBook(int id) {
    sqlite3* DB;
    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    string sql = "UPDATE BOOKS SET STATUS = 'FREE' WHERE ID = " + to_string(id) + ";";

    char* messageError;
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

    if (exit != SQLITE_OK) {
        cerr << "Error updating book status: " << messageError << endl;
        sqlite3_free(messageError);
    }
    else {
        cout << "Book returned successfully" << endl;
    }

    sqlite3_close(DB);
}

int dashboard::getTotalBooks() {
    sqlite3* DB;
    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return -1;
    }

    string sql = "SELECT COUNT(*) FROM BOOKS;";

    int totalBooks = 0;
    sqlite3_stmt* stmt;
    exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);
    if (exit != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(DB) << endl;
        return -1;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        totalBooks = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
    return totalBooks;
}

int dashboard::getTakenBooks() {
    sqlite3* DB;
    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return -1;
    }

    string sql = "SELECT COUNT(*) FROM BOOKS WHERE STATUS = 'TAKEN';";

    int takenBooks = 0;
    sqlite3_stmt* stmt;
    exit = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);
    if (exit != SQLITE_OK) {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(DB) << endl;
        return -1;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        takenBooks = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(DB);
    return takenBooks;
}

void dashboard::generateBooks() {
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

void dashboard::deleteBook() {
    char title[MAX_INPUT_CHARS + 1] = "\0";
    int titleCount = 0;

    Rectangle titleBox = { 300, 150, 225, 30 };  // Adjusted position to center
    Rectangle searchButton = { 300, 200, 100, 30 };  // Adjusted position to center
    Rectangle cancelButton = { 425, 200, 100, 30 };  // Adjusted position to center

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

                DrawTextEx(customFont, "Select a Book to Delete", { (float)screenWidth/ 2 - MeasureText("Select a Book to Delete", 40) / 2, 40}, 40, 2, GREEN);  // Centered and uniform size

                int yOffset = 100;
                for (size_t i = 0; i < results.size(); ++i) {
                    DrawTextEx(customFont, results[i].c_str(), { 300, static_cast<float>(yOffset) }, 20, 2, GREEN);  // Centered and uniform size
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

        DrawTextEx(customFont, "Delete Book", { 300, 40 }, 40, 2, GREEN);  // Centered and uniform size

        DrawTextEx(customFont, "Title:", { 220, 158 }, 20, 2, GREEN);  // Centered and uniform size
        DrawInputBox(titleBox, title, titleCount, mouseOnTitle, framesCounter);

        DrawRectangleRec(searchButton, DARKGRAY);
        DrawTextEx(customFont, "Search", { 310, 205 }, 20, 2, GREEN);  // Centered and uniform size

        DrawRectangleRec(cancelButton, DARKGRAY);
        DrawTextEx(customFont, "Cancel", { 440, 205 }, 20, 2, GREEN);  // Centered and uniform size

        EndDrawing();
    }
}



void dashboard::windowInit() {
    customFont = LoadFont("../assets/fonts/consola.ttf");

    const char* genres[] = { "Fiction", "Non-Fiction", "Science", "History", "Art" };
    numGenres = sizeof(genres) / sizeof(genres[0]);
    booksTaken = new int[numGenres];

    createDatabase();
    getBooksTaken(booksTaken, genres, numGenres);

    framesCounter = 0;
    showDropdown = false;

    int totalBooks = getTotalBooks();
    int takenBooks = getTakenBooks();

    float takenBooksAngle = 0.0f;

    while (!WindowShouldClose()) {
        framesCounter++;

        float progress = min(1.0f, (float)framesCounter / animationFrames);

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextEx(customFont, "DASHBOARD", { (800 - MeasureTextEx(customFont, "DASHBOARD", 80, -3).x) / 2, (float)450 / 8 - 50 }, 80, -3, GREEN);
        
        takenBooksAngle = min(360.0f * (takenBooks / (float)totalBooks), takenBooksAngle + 10.0f * progress);

        DrawCircleSector({ (float)screenWidth / 6, (float)screenHeight / 3 + 20 }, 70, 0, takenBooksAngle, 100, RED);
        DrawCircleSector({ (float)screenWidth / 6, (float)screenHeight / 3 + 20 }, 70, takenBooksAngle, 360, 100, GREEN);

        DrawTextEx(customFont, "Taken Books", { 10, 280 }, 20, -2, RED);
        DrawTextEx(customFont, TextFormat("%.0f%%", (takenBooks / (float)totalBooks) * 100), { 10, 300 }, 20, -2, RED);
        DrawTextEx(customFont, "Free Books", { 25.0f + MeasureTextEx(customFont, "Taken Books", 20, -2).x, 280 }, 20, -2, GREEN);
        DrawTextEx(customFont, TextFormat("%.0f%%", ((totalBooks - takenBooks) / (float)totalBooks) * 100), { 25.0f + MeasureTextEx(customFont, "Taken Books", 20, -2).x, 300 }, 20, -2, GREEN);

        float barWidth = 45.0f;
        float barSpacing = 25.0f;
        float graphStartX = (float)screenWidth / 3;
        float graphStartY = (float)screenHeight / 3 + 130.0f;
        float graphHeight = 100.0f;

        for (int i = 0; i < numGenres; i++) {
            float barHeight = (booksTaken[i] / maxBooks) * graphHeight * progress;
            DrawRectangle(graphStartX + i * (barWidth + barSpacing), graphStartY - barHeight, barWidth, barHeight, GREEN);
            DrawTextEx(customFont, TextFormat("%d", (int)(booksTaken[i] * progress)), { graphStartX + i * (barWidth + barSpacing), graphStartY - barHeight - 20 }, 20, -2, GREEN);
            DrawTextEx(customFont, genres[i], { graphStartX + i * (barWidth + barSpacing), graphStartY + 10 }, 15, -2, GREEN);
        }
        
        Rectangle buttonBounds = { (float)screenWidth - 150, 20, 120, 30 };
        if (CheckCollisionPointRec(GetMousePosition(), buttonBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            showDropdown = !showDropdown;
        }
        DrawRectangleRec(buttonBounds, showDropdown ? DARKGREEN : GREEN);
        DrawTextEx(customFont, "Menu", { buttonBounds.x + 10, buttonBounds.y + 5 }, 20, -2, BLACK);
        
        if (showDropdown) {
            if (dropdownHeight < maxDropdownHeight) {
                dropdownHeight += dropdownSpeed;
            }
        }
        else {
            if (dropdownHeight > 0.0f) {
                dropdownHeight -= dropdownSpeed;
            }
        }

        if (dropdownHeight > 0.0f) {
            Rectangle dropdownBounds = { (float)screenWidth - 150, 60, 120, dropdownHeight };
            DrawRectangleRec(dropdownBounds, DARKGREEN);
            if (dropdownHeight >= maxDropdownHeight) {
                Rectangle viewDBButton = { dropdownBounds.x + 10, dropdownBounds.y + 10, 100, 30 };
                DrawRectangleRec(viewDBButton, GREEN);
                DrawTextEx(customFont, "View All", { viewDBButton.x + 10, viewDBButton.y + 5 }, 20, -2, BLACK);
                if (CheckCollisionPointRec(GetMousePosition(), viewDBButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    viewDatabase();
                    framesCounter = 0;
                    takenBooksAngle = 0.0f;
                }
                
                Rectangle addBookButton = { dropdownBounds.x + 10, viewDBButton.y + 40, 100, 30 };
                DrawRectangleRec(addBookButton, GREEN);
                DrawTextEx(customFont, "Add Book", { addBookButton.x + 10, addBookButton.y + 5 }, 20, -2, BLACK);
                if (CheckCollisionPointRec(GetMousePosition(), addBookButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {                    
                    addBook();
                    framesCounter = 0;
                    takenBooksAngle = 0.0f;
                }
                
                Rectangle removeBookButton = { dropdownBounds.x + 10, addBookButton.y + 40, 100, 30 };
                DrawRectangleRec(removeBookButton, GREEN);
                DrawTextEx(customFont, "Del Book", { removeBookButton.x + 10, removeBookButton.y + 5 }, 20, -2, BLACK);
                if (CheckCollisionPointRec(GetMousePosition(), removeBookButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {                    
                    cout << "Book removed\n";
                    deleteBook();
                    framesCounter = 0;
                    takenBooksAngle = 0.0f;
                }
            }
        }

        EndDrawing();
    }

    UnloadFont(customFont);
    delete[] booksTaken;
}
