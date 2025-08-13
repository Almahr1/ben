#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "text_editor_functions.h"
#include "gap_buffer.h"

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

    new_line->gb = gap_buffer_create(strlen(content) + 16); // Extra space for editing
    if (new_line->gb == NULL) {
        free(new_line);
        perror("Gap buffer creation failed");
        exit(EXIT_FAILURE);
    }

    // Insert the content into the gap buffer
    gap_buffer_insert_string(new_line->gb, content);

    new_line->next = NULL;
    new_line->prev = NULL;
    return new_line;
}

Line* create_new_line_empty() {
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_line == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    new_line->gb = gap_buffer_create(16); // Start with small capacity
    if (new_line->gb == NULL) {
        free(new_line);
        perror("Gap buffer creation failed");
        exit(EXIT_FAILURE);
    }

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
        gap_buffer_destroy(temp->gb);
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
        char *line_text = line_to_string(current_line);
        if (line_text) {
            fprintf(file, "%s\n", line_text);
            free(line_text);
        }
        current_line = current_line->next;
    }

    fclose(file);
}

void loadFromFile(const char *filename, TextBuffer *buffer) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // File doesn't exist - create a new empty buffer with one line
        init_editor_buffer();
        Line *initial_line = create_new_line_empty();
        insert_line_at_end(buffer, initial_line);
        buffer->current_line_node = initial_line;
        buffer->current_col_offset = 0;
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

    // If file was empty, create at least one empty line
    if (buffer->head == NULL) {
        Line *initial_line = create_new_line_empty();
        insert_line_at_end(buffer, initial_line);
        buffer->current_line_node = initial_line;
        buffer->current_col_offset = 0;
    } else {
        // Set the cursor to the beginning of the file
        buffer->current_line_node = buffer->head;
        buffer->current_col_offset = 0;
    }
}

// Gap buffer helper functions for line operations
size_t line_get_length(const Line *line) {
    if (!line || !line->gb) return 0;
    return gap_buffer_length(line->gb);
}

char line_get_char_at(const Line *line, size_t position) {
    if (!line || !line->gb) return '\0';
    return gap_buffer_get_char_at(line->gb, position);
}

char* line_to_string(const Line *line) {
    if (!line || !line->gb) return NULL;
    return gap_buffer_to_string(line->gb);
}

void line_insert_char_at(Line *line, size_t position, char c) {
    if (!line || !line->gb) return;
    gap_buffer_move_cursor_to(line->gb, position);
    gap_buffer_insert_char(line->gb, c);
}

void line_insert_string_at(Line *line, size_t position, const char *str) {
    if (!line || !line->gb || !str) return;
    gap_buffer_move_cursor_to(line->gb, position);
    gap_buffer_insert_string(line->gb, str);
}

void line_delete_char_at(Line *line, size_t position) {
    if (!line || !line->gb) return;
    gap_buffer_move_cursor_to(line->gb, position);
    gap_buffer_delete_char(line->gb);
}

void line_delete_char_before(Line *line, size_t position) {
    if (!line || !line->gb || position == 0) return;
    gap_buffer_move_cursor_to(line->gb, position);
    gap_buffer_delete_char_before(line->gb);
}