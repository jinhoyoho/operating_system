#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

#define MAX_LINE_LENGTH 1024

char* strcasestr(const char* line, const char* pattern) {
    if (*pattern == '\0') {
        return (char*)line;
    }

    while (*line) {
        size_t i = 0;
        while (tolower(line[i]) == tolower(pattern[i])) {
            if (pattern[i + 1] == '\0') {
                return (char*)line;
            }
            i++;
        }
        line++;
    }

    return NULL;
}

void grep(const char *pattern, const char *filename, int show_line_numbers, int invert_match, int ignore_case) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("can't open file!\n");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int line_number = 1;
    while (fgets(line, sizeof(line), file) != NULL) {
        int matched = 0;
        if (ignore_case) {
            const char* pattern_to_compare = pattern;
            if (invert_match) {
                if (strcasestr(line, pattern_to_compare) == NULL) {
                    matched = 1;
                }
            } else {
                if (strcasestr(line, pattern_to_compare) != NULL) {
                    matched = 1;
                }
            }
        } else {
            if (invert_match) {
                if (strstr(line, pattern) == NULL) {
                    matched = 1;
                }
            } else {
                if (strstr(line, pattern) != NULL) {
                    matched = 1;
                }
            }
        }

        if (matched) {
            if (show_line_numbers) {
                printf("%d: %s", line_number, line);
            } else {
                printf("%s", line);
            }
        }
        line_number++;
    }

    fclose(file);
}
