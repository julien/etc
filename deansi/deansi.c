#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NORMAL,
    ESCAPE,
    SEQUENCE,
} State;

void strip_ansi(FILE *in, FILE *out) {
    State state = NORMAL;
    int c;

    while ((c = fgetc(in)) != EOF) {
        switch (state) {
            case NORMAL:
                if (c == 0x1b) {
                    state = ESCAPE;
                } else {
                    fputc(c, out);
                }
                break;
            case ESCAPE:
                if (c == '[') {
                    state = SEQUENCE;
                } else {
                    fputc(0x1b, out);
                    fputc(c, out);
                    state = NORMAL;
                }
                break;
            case SEQUENCE:
                // The final byte of a CSI sequence is in the range 0x40-0x7E.
                // Keep consuming bytes until we find one.
                if (c >= 0x40 && c <= 0x7e) {
                    state = NORMAL;
                }
                break;
        }
    }
}

void run_test(const char *name, const char *input, const char *expected) {
    char output_buf[1024] = {0};

    FILE *in_stream = fmemopen((void*)input, strlen(input), "r");
    FILE *out_stream = fmemopen(output_buf, sizeof(output_buf), "w");

    strip_ansi(in_stream, out_stream);

    fclose(in_stream);
    fclose(out_stream);

    if (strcmp(output_buf, expected) == 0) {
        printf("PASS: %s\n", name);
    } else {
        printf("FAIL: %s\n", name);
        printf("  Expected: '%s'\n", expected);
        printf("  Got:      '%s'\n", output_buf);
    }
}

static void run_all_tests(void) {
    printf("Running tests...\n");
    run_test(
        "removes a complex ansi sequence",
        "before \x1b[1;31mhello\x1b[0m after",
        "before hello after"
    );
    run_test(
        "handles non-ansi escape sequence",
        "this is not a sequence: \x1b_hello",
        "this is not a sequence: \x1b_hello"
    );
    printf("Tests finished.\n");
}

int main(int argc, char *argv[]) {
    if (argc == 2 && strcmp(argv[1], "test") == 0) {
        run_all_tests();
        return EXIT_SUCCESS;
    }

    if (argc < 2) {
        strip_ansi(stdin, stdout);
    } else {
        for (int i = 1; i < argc; i++) {
            FILE *f = fopen(argv[i], "r");
            if (f == NULL) {
                perror(argv[i]);
                continue;
            }
            strip_ansi(f, stdout);
            fclose(f);
        }
    }

    return EXIT_SUCCESS;
}
