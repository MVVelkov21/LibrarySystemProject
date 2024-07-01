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
    vector<pair<string, string>> records;
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
    int selectedIndex = 0;
    int scrollOffset = 0;
    const int maxVisibleLines = 20;
    const int lineHeight = 20;
    const int tableWidth = 800;
    const Vector2 mousePos = GetMousePosition();
    const float boxPadding = 2.0f;

    while (viewingDatabase && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        int startIndex = scrollOffset;
        int endIndex = min(scrollOffset + maxVisibleLines, static_cast<int>(records.size()));

        int yOffset = 20;
        for (int i = startIndex; i < endIndex; ++i) {            
            Vector2 textSize = MeasureTextEx(customFont, records[i].first.c_str(), 20, 1);
            Rectangle textBounds = { 20, static_cast<float>(yOffset), textSize.x, lineHeight };            
            if (CheckCollisionPointRec(mousePos, textBounds)) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {                    
                    int bookId = i + 1; 
                    string currentStatus = records[i].second;
                    string newStatus = (currentStatus == "FREE") ? "TAKEN" : "FREE";

                    if (sqlite3_open("books.db", &DB) == SQLITE_OK) {
                        string updateSql = "UPDATE BOOKS SET STATUS = '" + newStatus + "' WHERE ID = " + to_string(bookId) + ";";
                        char* errMsg;
                        if (sqlite3_exec(DB, updateSql.c_str(), NULL, 0, &errMsg) == SQLITE_OK) {
                            cout << "Book status updated successfully" << endl;                            
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
            Color textColor = (records[i].second == "TAKEN") ? RED : GREEN;
            DrawTextEx(customFont, records[i].first.c_str(), { 20, static_cast<float>(yOffset) }, 20, 1, textColor);

            yOffset += lineHeight;
        }        
        int scroll = GetMouseWheelMove();
        if (scroll != 0) {            
            selectedIndex -= scroll;
            if (selectedIndex < 0) {
                selectedIndex = 0;
            }
            else if (selectedIndex >= static_cast<int>(records.size())) {
                selectedIndex = static_cast<int>(records.size()) - 1;
            }            
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            }
            else if (selectedIndex >= scrollOffset + maxVisibleLines) {
                scrollOffset = selectedIndex - maxVisibleLines + 1;
            }
        }        
        if (selectedIndex >= startIndex && selectedIndex < endIndex) {
            Vector2 selectedTextSize = MeasureTextEx(customFont, records[selectedIndex].first.c_str(), 20, 1);
            Rectangle selectedTextBounds = { 20, static_cast<float>(20 + (selectedIndex - startIndex) * lineHeight), selectedTextSize.x, lineHeight };            
            DrawRectangleLinesEx({ selectedTextBounds.x - boxPadding, selectedTextBounds.y - boxPadding, selectedTextBounds.width + 2 * boxPadding, selectedTextBounds.height + 2 * boxPadding }, 2, GRAY);
        }

        if (IsKeyPressed(KEY_ENTER)) {            
            int bookId = selectedIndex + 1;
            string currentStatus = records[selectedIndex].second;
            string newStatus = (currentStatus == "FREE") ? "TAKEN" : "FREE";

            if (sqlite3_open("books.db", &DB) == SQLITE_OK) {
                string updateSql = "UPDATE BOOKS SET STATUS = '" + newStatus + "' WHERE ID = " + to_string(bookId) + ";";
                char* errMsg;
                if (sqlite3_exec(DB, updateSql.c_str(), NULL, 0, &errMsg) == SQLITE_OK) {
                    cout << "Book status updated successfully" << endl;                   
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

void dashboard::windowInit() {
    customFont = LoadFont("../assets/fonts/consola.ttf");

    book books(customFont, screenWidth, screenHeight);

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
                    books.addBook();
                    framesCounter = 0;
                    takenBooksAngle = 0.0f;
                }
                
                Rectangle removeBookButton = { dropdownBounds.x + 10, addBookButton.y + 40, 100, 30 };
                DrawRectangleRec(removeBookButton, GREEN);
                DrawTextEx(customFont, "Del Book", { removeBookButton.x + 10, removeBookButton.y + 5 }, 20, -2, BLACK);
                if (CheckCollisionPointRec(GetMousePosition(), removeBookButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {                    
                    cout << "Book removed\n";
                    books.deleteBook();
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
