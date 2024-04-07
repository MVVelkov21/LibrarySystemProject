#include "../libSysLib/precompiledHeader.h"

bool mainMenu::isUserExists(const string& username, sqlite3* db) {
    const char* selectQuery = "SELECT * FROM LoginInfo WHERE username = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, selectQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int result = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    return result == SQLITE_ROW;
}

void mainMenu::login(sqlite3* db) {
    string username, password;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    char ch;
    while (true) {
        ch = _getch();

        if (ch == '\r' || ch == '\n') {
            if (password.length() >= 6 &&
                std::any_of(password.begin(), password.end(), ::isdigit) &&
                std::any_of(password.begin(), password.end(), ::isupper) &&
                strpbrk(password.c_str(), "!@#$%^&*()-_+=<>?,./;:'\"[]{}\\|`~") != nullptr) {
                break;
            }
            else {
                std::cout << "\nInvalid password.";
                _getch();
                exit(0);
            }
        }
        else {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    cout << endl;

    if (isUserExists(username, db)) {
        cout << "User with the same username already exists. Please choose a different username." << endl;        
        return;
    }

    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS LoginInfo ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL,"
        "password TEXT NOT NULL);";

    if (sqlite3_exec(db, createTableQuery, nullptr, nullptr, nullptr) != SQLITE_OK) {
        cout << "Error creating table: " << sqlite3_errmsg(db) << endl;
        return;
    }

    const char* insertQuery = "INSERT INTO LoginInfo (username, password) VALUES (?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, insertQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        cout << "Error preparing statement" << endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Error executing statement" << endl;
    }
    else {
        cout << "User information saved to the database." << endl;
    }

    sqlite3_finalize(stmt);
}

void mainMenu::windowInit() {
    sqlite3* db;

    if (sqlite3_open("../assets/database/database.db", &db) != SQLITE_OK) {
        cout << "Error opening database" << endl;
    }

    InitWindow(screenWidth, screenHeight, "Library System");

    SetTargetFPS(60);

    Font customFont = LoadFont("../assets/fonts/consola.ttf");

    Rectangle buttonRect1 = { (screenWidth - 200) / 2, (screenHeight - 50) / 2 - 60, 200, 50 };
    Rectangle buttonRect2 = { (screenWidth - 200) / 2, (screenHeight - 50) / 2 + 50, 200, 50 };

    while (!WindowShouldClose()) {

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();

            if (CheckCollisionPointRec(mousePosition, buttonRect1)) {
                printf("Button 1 clicked!\n");
                login(db);
            }
            else if (CheckCollisionPointRec(mousePosition, buttonRect2)) {
                printf("Button 2 clicked!\n");
                
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        Vector2 mousePosition = GetMousePosition();
        bool button1Hovered = CheckCollisionPointRec(mousePosition, buttonRect1);
        bool button2Hovered = CheckCollisionPointRec(mousePosition, buttonRect2);

        
        DrawRectangleLinesEx(buttonRect1, 2, (button1Hovered ? GREEN : GRAY));
        DrawRectangleLinesEx(buttonRect2, 2, (button2Hovered ? GREEN : GRAY));

        
        if (button1Hovered)
            DrawRectangleRec(buttonRect1, Fade(GREEN, 0.3f));
        else
            DrawRectangleRec(buttonRect1, BLANK);

        if (button2Hovered)
            DrawRectangleRec(buttonRect2, Fade(GREEN, 0.3f));
        else
            DrawRectangleRec(buttonRect2, BLANK);

        DrawTextEx(customFont, "Library System", { (screenWidth - MeasureTextEx(customFont, "Library System", 40, -3).x) / 2, (float)screenHeight / 8 }, 40, -3, GREEN);
        DrawTextEx(customFont, "Login", { (screenWidth - MeasureTextEx(customFont, "Login", 30, -3).x) / 2, buttonRect1.y + (buttonRect1.height - 30) / 2 }, 30, -3, GREEN);
        DrawTextEx(customFont, "Register", { (screenWidth - MeasureTextEx(customFont, "Register", 30, -3).x) / 2, buttonRect2.y + (buttonRect2.height - 30) / 2 }, 30, -3, GREEN);

        EndDrawing();
    }

    CloseWindow();
}



