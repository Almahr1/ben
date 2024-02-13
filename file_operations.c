#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "text_editor_functions.h"

void saveToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        endwin();
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= current_line; ++i) {
        fprintf(file, "%s\n", buffer[i]);
    }

    fclose(file);
}

void loadFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        endwin();
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int line_count = 0;
    while (fgets(buffer[line_count], MAX_COLS, file) != NULL &&
            line_count < MAX_LINES) {
        buffer[line_count][strcspn(buffer[line_count], "\n")] = '\0';
        line_count++;
    }
    current_line = line_count - 1;
    current_col = strlen(buffer[current_line]) + 5;

    fclose(file);

    for (int i = 0; i < MAX_LINES; ++i) {
        line_numbers[i] = i + 1;
    }
}
