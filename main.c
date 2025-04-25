#include <sys/stat.h>

#include "raylib.h"

// get a file's modification time
long modTime(const char *name) {
	struct stat attr;
	if (stat(name, &attr) == 0)
		return attr.st_mtime;
	return 0;
}

int main(void) {
	const int width = 800;
	const int height = 500;
	const char *shaderPath = "shader.frag";

	InitWindow(width, height, ".~. adventure time");
	SetTargetFPS(60);

	Shader shader = LoadShader(0, shaderPath);
	long mTime = modTime(shaderPath);

	int timeLoc = GetShaderLocation(shader, "time");
	int resLoc = GetShaderLocation(shader, "resolution");

	float resolution[2] = {(float)width, (float)height};
	float startTime = (float)GetTime();

	while (!WindowShouldClose()) {
		long currTime = modTime(shaderPath);
		if (currTime > mTime) {
			mTime = currTime;
			UnloadShader(shader);

			shader = LoadShader(0, shaderPath);
			timeLoc = GetShaderLocation(shader, "time");
			resLoc = GetShaderLocation(shader, "resolution");
			// rotLoc = GetShaderLocation(shader, "rotation");

			TraceLog(LOG_INFO, "shader reloaded");
		}

		float time = (float)GetTime();
		float nextTime = (float)(time - startTime);

		SetShaderValue(shader, timeLoc, &nextTime, SHADER_UNIFORM_FLOAT);
		SetShaderValue(shader, resLoc, &resolution, SHADER_UNIFORM_VEC2);

		BeginDrawing();
		ClearBackground(BLACK);

		BeginShaderMode(shader);
		DrawRectangle(0, 0, width, height, WHITE);
		EndShaderMode();

		DrawFPS(10, 10);
		EndDrawing();
	}
	UnloadShader(shader);
	CloseWindow();
}
