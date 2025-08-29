#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SYMBOLS 11     // 0-9 and ":"
#define LINES_PER_SYMBOL 3
#define MAX_PART_LEN 5     // " |_| " + \0, etc...
#define TIME_BUF_SIZE 9    // "HH:MM:SS\0"
#define LINE_BUF_SIZE 256

int get_symbol_index(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c == ':')
		return 10;
	return -1;
}

int main(void) {
	const char *ledmap[SYMBOLS] = {
		" _  ,| | ,|_| ", // 0
		"  ,| ,| ",       // 1
		" _  , _| ,|_  ", // 2
		"_  ,_| ,_| ",    // 3
		"    ,|_| ,  | ", // 4
		" _  ,|_  , _| ", // 5
		" _  ,|_  ,|_| ", // 6
		"_   , |  , |  ", // 7
		" _  ,|_| ,|_| ", // 8
		" _  ,|_| , _| ", // 9
		"    , .  , .  ", // :
	};

	// A 3D array to hold the parsed parts of the LED symbols.
	// e.g., led_parts[0][0] will be " _  "
	char ledparts[SYMBOLS][LINES_PER_SYMBOL][MAX_PART_LEN];

	for (int i = 0; i < SYMBOLS; i++) {
		char *copy = strdup(ledmap[i]);
		if (!copy) exit(EXIT_FAILURE);

		// A pointer for strsep to walk through the string
		char *p = copy;

		for (int j = 0; j < LINES_PER_SYMBOL; j++) {
			char *token = strsep(&p, ",");
			if (!token) continue;

			strncpy(ledparts[i][j], token, MAX_PART_LEN - 1);
			ledparts[i][j][MAX_PART_LEN - 1] = '\0';
		}
		free(copy);
	}

	char timestr[TIME_BUF_SIZE];
	char line1[LINE_BUF_SIZE], line2[LINE_BUF_SIZE], line3[LINE_BUF_SIZE];

	while (1) {
		time_t rawtime;
		struct tm *timeinfo;
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		if (!timeinfo) exit(EXIT_FAILURE);

		strftime(timestr, TIME_BUF_SIZE, "%H:%M:%S", timeinfo);

		// Reset line buffers for the new time
		line1[0] = '\0';
		line2[0] = '\0';
		line3[0] = '\0';

		for (size_t i = 0; i < strlen(timestr); i++) {
			int index = get_symbol_index(timestr[i]);
			if (index == -1) continue;

			strncat(line1, ledparts[index][0], LINE_BUF_SIZE - strlen(line1) - 1);
			strncat(line2, ledparts[index][1], LINE_BUF_SIZE - strlen(line2) - 1);
			strncat(line3, ledparts[index][2], LINE_BUF_SIZE - strlen(line3) - 1);
		}

		printf("\x1b[2J\x1b[H");
		printf("%s\n%s\n%s\n", line1, line2, line3);
		fflush(stdout);

		sleep(1);
	}

	return EXIT_SUCCESS;
}
