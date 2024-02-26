#include "../libSysLib/precompiledHeader.h"

void mainMenu::windowInit() {
    sqlite3* db;

    if (sqlite3_open("database.db", &db) != SQLITE_OK) {
        cout << "Error opening database" << endl;
    }

    InitWindow(screenWidth, screenHeight, "Library System");

    Font font = LoadFont("../assets/consola.ttf");
    
    SetTargetFPS(240);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawTextEx(font,"Congrats! You created your first window!", {190, 200}, 20, 0, GREEN);

        EndDrawing();
    }
    CloseWindow();
}