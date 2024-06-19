#include "../libSysLib/dashboard.h"

void dashboard::windowInit() {
	Font customFont = LoadFont("../assets/fonts/consola.ttf");

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		DrawTextEx(customFont, "DASHBOARD", { (800 - MeasureTextEx(customFont, "DASHBOARD", 80, -3).x) / 2, (float)450 / 8 - 50 }, 80, -3, GREEN);

		EndDrawing();
	}

	UnloadFont(customFont);
}