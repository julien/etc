#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1024
#define HEIGHT 1024

#define MAX_POINTS 8000
#define POINT_SIZE 7
const unsigned int numPoints = MAX_POINTS * POINT_SIZE;

#define MAX_BLOBS 5
struct blob {
	float cx;
	float cy;
	float cr;
	float nx;
	float ny;
	float nr;
	float ex;
	float ey;
	float er;
};

void updatePoints(float *points, struct blob *blobs) {
	for (int i = 0; i < numPoints; i += POINT_SIZE) {
		float x = points[i + 0];
		float y = points[i + 1];
		float r = points[i + 2];
		float vx = points[i + 3];
		float vy = points[i + 4];
		float ax = points[i + 5];
		float ay = points[i + 6];

		vx += ax;
		vy += ay;
		ax *= 0;
		ay *= 0;

		for (int k = 0; k < MAX_BLOBS; k++) {
			struct blob b = blobs[k];
			float dx = x - b.cx;
			float dy = y - b.cy;
			float dist = sqrt(dx * dx + dy * dy);

			if (dist < b.cr) {
				x = (b.cx + dx / dist * b.cr) * 0.09;
				y = (b.cy + dy / dist * b.cr) * 0.09;
				r = (10 + rand() % 20) * 0.1;
			}
		}

		x += vx;
		y += vy;
		if (x > WIDTH) {
			x = 0.0;
		} else if (x < 0.0) {
			x = WIDTH;
		}
		if (y > HEIGHT) {
			y = 0.0;
		} else if (y < 0.0) {
			y = HEIGHT;
		}

		points[i + 0] = x;
		points[i + 1] = y;
		points[i + 2] = r;
	}

	for (int i = 0; i < MAX_BLOBS; i++) {
		float dx = blobs[i].nx - blobs[i].cx;
		float dy = blobs[i].ny - blobs[i].cy;
		float dr = blobs[i].nr - blobs[i].cr;

		float ax = fabs(dx);
		float ay = fabs(dy);
		float ar = fabs(dr);

		if (ax > 0.01 && ay > 0.01 && ar > 0.01) {
			dx *= blobs[i].ex;
			dy *= blobs[i].ey;
			dr *= blobs[i].er;

			blobs[i].cx += dx * GetRandomValue(3, 9) * 0.01;
			blobs[i].cy += dy * GetRandomValue(3, 9) * 0.01;
			blobs[i].cr += dr * GetRandomValue(6, 9) * 0.1;
		} else {
			blobs[i].ex = GetRandomValue(1, 3) * 0.08;
			blobs[i].ey = GetRandomValue(1, 3) * 0.08;
			blobs[i].er = GetRandomValue(1, 3) * 0.08;

			blobs[i].nx = GetRandomValue(0, WIDTH);
			blobs[i].ny = GetRandomValue(0, HEIGHT);
			blobs[i].nr = GetRandomValue(20, 100);
		}
	}
}

int main(void) {
	InitWindow(WIDTH, HEIGHT, "");
	SetTargetFPS(60);

	Shader shader = LoadShader("blobs.vert", "blobs.frag");
	int timeLoc = GetShaderLocation(shader, "time");
	int resLoc = GetShaderLocation(shader, "resolution");

	float resolution[2] = {(float)WIDTH, (float)HEIGHT};
	float startTime = (float)GetTime();

	struct blob blobs[MAX_BLOBS] = {0};
	for (int i = 0; i < MAX_BLOBS; i++) {
		blobs[i].cx = GetRandomValue(0, WIDTH);
		blobs[i].cy = GetRandomValue(0, HEIGHT);
		blobs[i].cr = GetRandomValue(20, 100);

		blobs[i].nx = GetRandomValue(0, WIDTH);
		blobs[i].ny = GetRandomValue(0, HEIGHT);
		blobs[i].nr = GetRandomValue(20, 100);

		blobs[i].ex = GetRandomValue(1, 3) * 0.08;
		blobs[i].ey = GetRandomValue(1, 3) * 0.08;
		blobs[i].er = GetRandomValue(1, 3) * 0.08;
	}

	float points[numPoints] = {0};
	for (int i = 0; i < numPoints; i += POINT_SIZE) {
		points[i + 0] = (float)GetRandomValue(10, WIDTH - 10);
		points[i + 1] = (float)GetRandomValue(10, HEIGHT - 10);
		points[i + 2] = (10 + rand() % 20) * 0.1;
		points[i + 3] = GetRandomValue(-3, 3);
		points[i + 4] = GetRandomValue(-3, 3);
		points[i + 5] = GetRandomValue(-2, 2) * 0.1;
		points[i + 6] = GetRandomValue(-2, 2) * 0.1;
	}

	unsigned int stride = sizeof(float) * POINT_SIZE;
	int loc = shader.locs[SHADER_LOC_VERTEX_POSITION];

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numPoints, points,
	             GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, stride, (void *)0);
	glEnableVertexAttribArray(0);

	loc = GetShaderLocationAttrib(shader, "pointSize");
	glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, stride,
	                      (void *)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	glUseProgram(shader.id);
	glEnable(GL_PROGRAM_POINT_SIZE);

	Matrix modelViewProjection =
	    MatrixMultiply(rlGetMatrixModelview(), rlGetMatrixProjection());
	glUniformMatrix4fv(shader.locs[SHADER_LOC_MATRIX_MVP], 1, false,
	                   MatrixToFloat(modelViewProjection));

	while (!WindowShouldClose()) {
		updatePoints(points, blobs);

		float time = (float)GetTime();
		float nextTime = (float)(time - startTime);

		SetShaderValue(shader, timeLoc, &nextTime,
		               SHADER_UNIFORM_FLOAT);
		SetShaderValue(shader, resLoc, &resolution,
		               SHADER_UNIFORM_VEC2);

		BeginDrawing();
		DrawFPS(10, 10);

		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(shader.id);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * numPoints,
		                points);

		glDrawArrays(GL_POINTS, 0, MAX_POINTS);
		glBindVertexArray(0);
		EndDrawing();
	}
	UnloadShader(shader);
	CloseWindow();
}
