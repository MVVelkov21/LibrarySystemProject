#include "../libSysLib/mainMenu.h"


void mainMenu::windowInit() {
    InitWindow(dash.screenWidth, dash.screenHeight, "Library System");

    SetTargetFPS(60);

    Font customFont = LoadFont("../assets/fonts/consola.ttf");
    
    Rectangle buttonRect1 = { (dash.screenWidth - 400) / 2, (dash.screenHeight - 100) / 2 + 100, 400, 100 };

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePosition = GetMousePosition();

            if (CheckCollisionPointRec(mousePosition, buttonRect1)) {
                printf("Button 1 clicked!\n");
                dash.windowInit();
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        Vector2 mousePosition = GetMousePosition();
        bool button1Hovered = CheckCollisionPointRec(mousePosition, buttonRect1);

        DrawRectangleLinesEx(buttonRect1, 2, (button1Hovered ? GREEN : GRAY));

        if (button1Hovered)
            DrawRectangleRec(buttonRect1, Fade(GREEN, 0.3f));
        else
            DrawRectangleRec(buttonRect1, BLANK);
        
        Vector2 titlePosition = { (dash.screenWidth - MeasureTextEx(customFont, "Library System", 100, -3).x) / 2, (float)dash.screenHeight / 8 };
        DrawTextEx(customFont, "Library System", titlePosition, 100, -3, GREEN);
        
        Vector2 buttonTextPosition = { (dash.screenWidth - MeasureTextEx(customFont, "ENTER", 60, -3).x) / 2, buttonRect1.y + (buttonRect1.height - 60) / 2 };
        DrawTextEx(customFont, "ENTER", buttonTextPosition, 60, -3, GREEN);

        EndDrawing();
    }

    CloseWindow();
}




