#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <string.h>

static inline int read_integer(const char *prompt, int *value) {
    int result;
    int ch;

    for (;;) {
        printf("%s", prompt);
        result = scanf("%d", value);

        if (result == 1) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            return 1;
        }

        if (result == EOF) {
            return 0;
        }

        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        printf("Invalid input. Please enter a whole number.\n");
    }
}

static inline int read_double(const char *prompt, double *value) {
    int result;
    int ch;

    for (;;) {
        printf("%s", prompt);
        result = scanf("%lf", value);

        if (result == 1) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            return 1;
        }

        if (result == EOF) {
            return 0;
        }

        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        printf("Invalid input. Please enter a number.\n");
    }
}

static inline int read_line(const char *prompt, char *buffer, size_t size) {
    size_t length;
    int ch;

    printf("%s", prompt);
    if (!fgets(buffer, (int)size, stdin)) {
        return 0;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
        return 1;
    }

    if (feof(stdin)) {
        return 1;
    }
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
    printf("Input is too long. Please try again.\n");
    return 0;
}

#endif
