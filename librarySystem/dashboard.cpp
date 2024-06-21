#include "../libSysLib/dashboard.h"

void createDatabase() {
    sqlite3* DB;
    int exit = 0;
    exit = sqlite3_open("books.db", &DB);

    string sql = "CREATE TABLE IF NOT EXISTS BOOKS("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "ISBN TEXT NOT NULL, "
        "TITLE TEXT NOT NULL, "
        "AUTHOR TEXT NOT NULL, "
        "GENRE TEXT NOT NULL, "
        "PUBLICATION_DATE TEXT NOT NULL);";

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

void insertRandomBooks() {
    sqlite3* DB;
    sqlite3_stmt* stmt;
    int exit = sqlite3_open("books.db", &DB);
    if (exit) {
        cerr << "Can't open database: " << sqlite3_errmsg(DB) << endl;
        return;
    }
    
    string checkSql = "SELECT COUNT(*) FROM BOOKS;";
    sqlite3_prepare_v2(DB, checkSql.c_str(), -1, &stmt, 0);
    int bookCount = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        bookCount = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (bookCount == 0) {        
        srand(time(0));
        vector<string> genres = { "Fiction", "Non-Fiction", "Science", "History", "Art" };
        for (int i = 0; i < 100; ++i) {
            string isbn = "978-" + to_string(rand() % 1000000);
            string title = "Book " + to_string(i + 1);
            string author = "Author " + to_string(rand() % 50 + 1);
            string genre = genres[rand() % genres.size()];
            string pubDate = "20" + to_string(rand() % 23 + 1) + "-01-01";

            string sql = "INSERT INTO BOOKS (ISBN, TITLE, AUTHOR, GENRE, PUBLICATION_DATE) VALUES ('" +
                isbn + "', '" + title + "', '" + author + "', '" + genre + "', '" + pubDate + "');";

            char* messageError;
            exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);

            if (exit != SQLITE_OK) {
                cerr << "Error Inserting Data: " << messageError << endl;
                sqlite3_free(messageError);
            }
        }
        cout << "100 random books inserted successfully" << endl;
    }
    else {
        cout << "Books already exist in the database. No need to insert." << endl;
    }

    sqlite3_close(DB);
}

void getBooksTaken(int* booksTaken, const char** genres, int numGenres) {
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

void dashboard::windowInit() {
    Font customFont = LoadFont("../assets/fonts/consola.ttf");

    float takenBooks = 20.0f;
    float totalBooks = 100.0f;

    float takenBooksAngle = 360.0f * (takenBooks / totalBooks);
    
    const char* genres[] = { "Fiction", "Non-Fiction", "Science", "History", "Art" };
    int numGenres = sizeof(genres) / sizeof(genres[0]);
    int* booksTaken = new int[numGenres];
    float maxBooks = 20.0f; // Maximum number of books taken for scaling

    createDatabase();
    insertRandomBooks();
    getBooksTaken(booksTaken, genres, numGenres);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTextEx(customFont, "DASHBOARD", { (800 - MeasureTextEx(customFont, "DASHBOARD", 80, -3).x) / 2, (float)450 / 8 - 50 }, 80, -3, GREEN);

        DrawCircleSector({ (float)screenWidth / 6, (float)screenHeight / 3 }, 50, 0, takenBooksAngle, 100, RED);
        DrawCircleSector({ (float)screenWidth / 6, (float)screenHeight / 3 }, 50, takenBooksAngle, 360, 100, GREEN);
        
        DrawTextEx(customFont, "Taken Books", { 10, 200 }, 20, -2, RED);
        DrawTextEx(customFont, TextFormat("%.0f%%", (takenBooks / totalBooks) * 100), { 10, 220 }, 20, -2, RED);
        DrawTextEx(customFont, "Free Books", { 25.0f + MeasureTextEx(customFont, "Taken Books", 20, -2).x, 200 }, 20, -2, GREEN);
        DrawTextEx(customFont, TextFormat("%.0f%%", ((totalBooks - takenBooks) / totalBooks) * 100), { 25.0f + MeasureTextEx(customFont, "Taken Books", 20, -2).x, 220 }, 20, -2, GREEN);
        
        float barWidth = 40.0f;
        float barSpacing = 20.0f;
        float graphStartX = (float)screenWidth / 2;
        float graphStartY = (float)screenHeight / 3 + 150.0f;
        float graphHeight = 200.0f;

        for (int i = 0; i < numGenres; i++) {
            float barHeight = (booksTaken[i] / maxBooks) * graphHeight;
            DrawRectangle(graphStartX + i * (barWidth + barSpacing), graphStartY - barHeight, barWidth, barHeight, BLUE);
            DrawTextEx(customFont, TextFormat("%d", booksTaken[i]), { graphStartX + i * (barWidth + barSpacing), graphStartY - barHeight - 20 }, 20, -2, WHITE);
            DrawTextEx(customFont, genres[i], { graphStartX + i * (barWidth + barSpacing), graphStartY + 10 }, 15, -2, WHITE);
        }

        EndDrawing();
    }

    UnloadFont(customFont);
}
