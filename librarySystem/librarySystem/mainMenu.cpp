#include "../libSysLib/precompiledHeader.h"

struct UserInfo {
    std::string username;
    std::string password;
};

void insertUserInfo(sqlite3* db, const UserInfo& user) {
    std::string query = "INSERT INTO users (username, password) VALUES (?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, user.password.c_str(), -1, SQLITE_STATIC);       

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    else {
        std::cout << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
    }
}


std::string GetTextInput(const Vector2& position, const std::string& label) {
    const int maxInputLength = 17;
    char input[maxInputLength] = { 0 };
    int currentLength = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);
       
        DrawText(label.c_str(), position.x, position.y, 20, GREEN);
        DrawRectangleLines(position.x, position.y + 30, 200, 30, GREEN);
        DrawText(input, position.x + 5, position.y + 35, 20, GREEN);

        EndDrawing();
        
        if (IsKeyPressed(KEY_ENTER)) {
            return input;
        }
        else if (IsKeyPressed(KEY_BACKSPACE) && currentLength > 0) {
            input[--currentLength] = '\0';
        }
        else if (currentLength < maxInputLength - 1) {            
            for (int key = KEY_ZERO; key <= KEY_NINE; key++) {
                if (IsKeyPressed(key)) {
                    input[currentLength++] = (char)(key - KEY_ZERO + '0');
                }
            }
            for (int key = KEY_A; key <= KEY_Z; key++) {
                if (IsKeyPressed(key)) {
                    input[currentLength++] = (char)(key - KEY_A + 'a');
                }
            }
        }
    }

    return "";
}

void mainMenu::windowInit() {
    sqlite3* db;

    if (sqlite3_open("../assets/database/database.db", &db) != SQLITE_OK) {
        std::cout << "Error opening database" << std::endl;
        return;
    }

    std::string query = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT)";

    if (sqlite3_exec(db, query.c_str(), NULL, 0, NULL) != SQLITE_OK) {
        std::cout << "Error creating users table: " << sqlite3_errmsg(db) << std::endl;
    }

    InitWindow(screenWidth, screenHeight, "Library System");

    Font font = LoadFont("../assets/fonts/consola.ttf");

    SetTargetFPS(240);

    std::string username;
    std::string password;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawTextEx(font, "Registration Form", { 190, 150 }, 20, 0, GREEN);
        DrawTextEx(font, "Username:", { 190, 200 }, 20, 0, GREEN);
        DrawTextEx(font, "Password:", { 190, 240 }, 20, 0, GREEN);
       
        username = GetTextInput({ 250, 200 }, "Username:");
        password = GetTextInput({ 250, 240 }, "Password:");

        if (!username.empty() && !password.empty() && IsKeyPressed(KEY_ENTER)) {
            UserInfo newUser;
            newUser.username = username;
            newUser.password = password;
            
            insertUserInfo(db, newUser);
            
            std::cout << "User registered successfully!" << std::endl;
        }

        EndDrawing();
    }

    CloseWindow();
    sqlite3_close(db);
}
