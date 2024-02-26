#include "../libSysLib/precompiledHeader.h"

void mainMenu::windowInit() {
    if (sqlite3_open("database.db", &db) != SQLITE_OK) {
        cout << "Error opening database" << endl;
    }

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(240);
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }
    CloseWindow();
}