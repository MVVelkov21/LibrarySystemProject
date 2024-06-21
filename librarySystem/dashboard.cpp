#include "../libSysLib/dashboard.h"

void dashboard::windowInit() {
	Font customFont = LoadFont("../assets/fonts/consola.ttf");

    float takenBooks = 20.0f;    
    float totalBooks = 100.0f;

    float takenBooksAngle = 360.0f * (takenBooks / totalBooks);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		DrawTextEx(customFont, "DASHBOARD", { (800 - MeasureTextEx(customFont, "DASHBOARD", 80, -3).x) / 2, (float)450 / 8 - 50 }, 80, -3, GREEN);        

        DrawCircleSector({(float)screenWidth / 6, (float)screenHeight / 3}, 50, 0, takenBooksAngle, 100, RED);
        DrawCircleSector({ (float)screenWidth / 6, (float)screenHeight / 3 }, 50, takenBooksAngle, 360, 100, GREEN);

        DrawTextEx(customFont, "Taken Books", { 10, 200 }, 20, -2, RED);
        DrawTextEx(customFont, TextFormat("%.0f%%", (takenBooks / totalBooks) * 100), { 10, 220 }, 20, -2, RED);
        DrawTextEx(customFont, "Free Books", { 25.0f + MeasureTextEx(customFont, "Taken Books", 20, -2).x, 200 }, 20, -2, GREEN);
        DrawTextEx(customFont, TextFormat("%.0f%%", ((totalBooks - takenBooks) / totalBooks) * 100), { 25.0f + MeasureTextEx(customFont, "Taken Books", 20, -2).x, 220 }, 20, -2, GREEN);


		EndDrawing();
	}

	UnloadFont(customFont);
}