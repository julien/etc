#include "raylib.h"

int main(void) {
	const int width = 800;
	const int height = 500;

	InitWindow(width, height, "Hello raylib");
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("Hello Raylib!", 350, 250, 20, LIGHTGRAY);
		EndDrawing();
	}
	CloseWindow();
}
