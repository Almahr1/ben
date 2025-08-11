#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "text_editor_functions.h"

// Define a reasonable initial capacity for a line
#define INITIAL_LINE_CAPACITY 256

// Initialize the global text buffer
TextBuffer editor_buffer;

void insert_line_after(Line *prev_line, Line *new_line) {
    if (prev_line == NULL) {
        // This case should ideally be handled by insert_line_at_end if it's the first line.
        // For safety, let's just do nothing or handle it as an error.
        return;
    }

    // Link the new line to the line after the previous line
    new_line->next = prev_line->next;

    // Link the previous line to the new line
    prev_line->next = new_line;
    new_line->prev = prev_line;

    // If there was a line after prev_line, update its 'prev' pointer
    if (new_line->next != NULL) {
        new_line->next->prev = new_line;
    }

    editor_buffer.num_lines++;

    // If the new line is inserted at the end, update the tail
    if (prev_line == editor_buffer.tail) {
        editor_buffer.tail = new_line;
    }
}

void init_editor_buffer() {
    editor_buffer.head = NULL;
    editor_buffer.tail = NULL;
    editor_buffer.num_lines = 0;
    editor_buffer.current_line_node = NULL;
    editor_buffer.current_col_offset = 0;
}

Line* create_new_line(const char *content) {
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_line == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    new_line->length = strlen(content);
    new_line->capacity = new_line->length + 1;
    new_line->text = (char *)malloc(new_line->capacity);
    if (new_line->text == NULL) {
        free(new_line);
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strcpy(new_line->text, content);

    new_line->next = NULL;
    new_line->prev = NULL;
    return new_line;
}

void insert_line_at_end(TextBuffer *buffer, Line *new_line) {
    if (buffer->tail == NULL) {
        buffer->head = new_line;
        buffer->tail = new_line;
    } else {
        buffer->tail->next = new_line;
        new_line->prev = buffer->tail;
        buffer->tail = new_line;
    }
    buffer->num_lines++;
}

void free_editor_buffer(TextBuffer *buffer) {
    Line *current = buffer->head;
    while (current != NULL) {
        Line *temp = current;
        current = current->next;
        free(temp->text);
        free(temp);
    }
    buffer->head = NULL;
    buffer->tail = NULL;
    buffer->num_lines = 0;
    buffer->current_line_node = NULL;
    buffer->current_col_offset = 0;
}

void saveToFile(const char *filename, TextBuffer *buffer) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        // Handle error gracefully
        return;
    }

    Line *current_line = buffer->head;
    while (current_line != NULL) {
        fprintf(file, "%s\n", current_line->text);
        current_line = current_line->next;
    }

    fclose(file);
}

void loadFromFile(const char *filename, TextBuffer *buffer) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // Handle error gracefully, maybe initialize an empty buffer
        init_editor_buffer();
        return;
    }


    // Free any existing content
    free_editor_buffer(buffer);

    char *line_buffer = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line_buffer, &len, file)) != -1) {
        // Remove the newline character if it exists
        if (line_buffer[read - 1] == '\n') {
            line_buffer[read - 1] = '\0';
        }
        Line *new_line = create_new_line(line_buffer);
        insert_line_at_end(buffer, new_line);
    }

    free(line_buffer); // Free the buffer allocated by getline
    fclose(file);

    // Set the cursor to the beginning of the file
    if (buffer->head != NULL) {
        buffer->current_line_node = buffer->head;
        buffer->current_col_offset = 0;
    }
}
