#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 1024
#define HEIGHT 1024
#define SHADER_MAX_LENGTH 262144

void sizeCallback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}

void print_infolog(GLuint index) {
	int max_length = 2048;
	int actual_length = 0;
	char log[2048];
	glGetShaderInfoLog(index, max_length, &actual_length, log);
	fprintf(stderr, "%u:\n%s\n", index, log);
}

int file_to_str(const char *file_name, char *shader_str, int max_len) {
	FILE *file = fopen(file_name, "r");
	if (!file) {
		fprintf(stderr, "couldn't open file %s", file_name);
		return 0;
	}

	size_t cnt = fread(shader_str, 1, max_len - 1, file);
	if ((int)cnt >= max_len - 1) {
		fprintf(stderr, "file %s too big", file_name);
		return 0;
	}

	if (ferror(file)) {
		fprintf(stderr, "couldn't read shader %s", file_name);
		fclose(file);
		return 0;
	}

	/* append \0 to the end of string */
	shader_str[cnt] = 0;

	fclose(file);
	return 1;
}

int create_shader(const char *file_name, GLuint *shader, GLenum type) {
	char shader_string[SHADER_MAX_LENGTH];
	file_to_str(file_name, shader_string, SHADER_MAX_LENGTH);

	*shader = glCreateShader(type);
	const GLchar *p = (const GLchar *)shader_string;
	glShaderSource(*shader, 1, &p, NULL);
	glCompileShader(*shader);

	int params = -1;
	glGetShaderiv(*shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "couldn't compile shader %i", *shader);
		print_infolog(*shader);
		return 0;
	}
	return 1;
}

GLuint create_program(const char *vert_src, const char *frag_src) {
	GLuint vert, frag;
	create_shader(vert_src, &vert, GL_VERTEX_SHADER);
	create_shader(frag_src, &frag, GL_FRAGMENT_SHADER);
	GLuint prog = glCreateProgram();
	glAttachShader(prog, vert);
	glAttachShader(prog, frag);
	glLinkProgram(prog);
	GLint params = -1;
	glGetProgramiv(prog, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "couldn't link shader program %u", prog);
		print_infolog(prog);
		return -1;
	}
	glDeleteShader(vert);
	glDeleteShader(frag);
	return prog;
}

float rand_range(float min, float max) {
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

#define MAX_POINTS 10000
#define POINT_SIZE 7
const unsigned int numPoints = MAX_POINTS * POINT_SIZE;

#define MAX_BLOBS 4
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

void update(struct blob *blobs, float *points) {
	for (int i = 0; i < MAX_BLOBS; i++) {
		float dx = blobs[i].nx - blobs[i].cx;
		float dy = blobs[i].ny - blobs[i].cy;
		float dr = blobs[i].nr - blobs[i].cr;

		float ax = fabs(dx);
		float ay = fabs(dy);
		float ar = fabs(dr);

		float vx = dx * blobs[i].ex;
		float vy = dy * blobs[i].ey;
		float vr = dr * blobs[i].er;

		blobs[i].cx += vx;
		blobs[i].cy += vy;
		blobs[i].cr += vr;

		if (ax < 0.01 && ay < 0.01 && ar < 0.01) {
			blobs[i].ex = rand_range(2, 6) * 0.009;
			blobs[i].ey = rand_range(2, 6) * 0.009;
			blobs[i].er = rand_range(2, 6) * 0.01;

			blobs[i].nx = rand_range(-10, 10) * 0.1;
			blobs[i].ny = rand_range(-10, 10) * 0.1;
			blobs[i].nr = rand_range(2, 30) * 0.01;
		}
	}

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
				x = b.cx + dx / dist * b.cr;
				y = b.cy + dy / dist * b.cr;
				r = (10 + rand() % 40) * 0.1;
			}
		}

		x += vx * 0.01;
		y += vy * 0.01;

		if (x > 1.0) {
			x = -1.0;
		} else if (x < -1.0) {
			x = 1.0;
		}
		if (y > 1.0) {
			y = -1.0;
		} else if (y < -1.0) {
			y = 1.0;
		}

		points[i + 0] = x;
		points[i + 1] = y;
		points[i + 2] = r;
	}
}

int main(void) {
	srand(time(NULL));

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *win = glfwCreateWindow(WIDTH, HEIGHT, "", NULL, NULL);
	if (!win) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	const GLFWvidmode *mode = glfwGetVideoMode(mon);

	int x = (int)((mode->width - WIDTH) * 0.5);
	int y = (int)((mode->height - HEIGHT) * 0.5);

	glfwSetWindowPos(win, x, y);
	glfwSetFramebufferSizeCallback(win, sizeCallback);

	glfwMakeContextCurrent(win);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1);

	GLuint prog = create_program("blobs.vert", "blobs.frag");

	struct blob blobs[MAX_BLOBS] = {0};
	for (int i = 0; i < MAX_BLOBS; i++) {
		blobs[i].cx = 0.0 + (i * 0.1);
		blobs[i].cy = 0.0 + (i * 0.1);
		blobs[i].cr = rand_range(0.1, 3) * 0.1;
		blobs[i].nx = rand_range(-10, 10) * 0.1;
		blobs[i].ny = rand_range(-10, 10) * 0.1;
		blobs[i].nr = rand_range(0.1, 3) * 0.1;
		blobs[i].ex = rand_range(2, 6) * 0.01;
		blobs[i].ey = rand_range(2, 6) * 0.01;
		blobs[i].er = rand_range(2, 6) * 0.01;
	}

	float points[numPoints] = {0};
	for (int i = 0; i < numPoints; i += POINT_SIZE) {
		points[i + 0] = rand_range(-10, 10) * 0.1;
		points[i + 1] = rand_range(-10, 10) * 0.1;
		points[i + 2] = (10 + rand() % 80) * 0.1;
		points[i + 3] = rand_range(-3, 3) * 0.0001;
		points[i + 4] = rand_range(-3, 3) * 0.0001;
		points[i + 5] = rand_range(-2, -9) * 0.1;
		points[i + 6] = rand_range(-2, -9) * 0.1;
	}

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numPoints, points,
	             GL_STATIC_DRAW);

	int stride = sizeof(float) * POINT_SIZE;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, NULL);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride,
	                      (void *)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	glUseProgram(prog);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0, 0.0, 0.0, 1.0);

	while (!glfwWindowShouldClose(win)) {
		int w, h;
		glfwGetFramebufferSize(win, &w, &h);
		glViewport(0, 0, w, h);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update(blobs, points);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * numPoints,
		                points);

		glDrawArrays(GL_POINTS, 0, MAX_POINTS);

		glfwPollEvents();
		glfwSwapBuffers(win);
	}

	glfwDestroyWindow(win);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
