#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "text_editor_functions.h"

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
    editor_buffer.state = create_editor_state();
    
    if (!editor_buffer.state) {
        fprintf(stderr, "Failed to initialize editor state\n");
        exit(EXIT_FAILURE);
    }
}

Line* create_new_line(const char *content) {
    Line *new_line = (Line *)malloc(sizeof(Line));
    if (new_line == NULL) {
        return NULL; // Return NULL instead of exiting
    }

    new_line->text_buffer = create_gap_buffer(INITIAL_BUFFER_SIZE);
    if (new_line->text_buffer == NULL) {
        free(new_line);
        return NULL;
    }
    
    // Insert the content into the gap buffer
    if (content && strlen(content) > 0) {
        gap_buffer_insert_string(new_line->text_buffer, content);
        gap_buffer_move_cursor(new_line->text_buffer, 0); // Move cursor to start
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
        free_gap_buffer(temp->text_buffer);
        free(temp);
    }
    buffer->head = NULL;
    buffer->tail = NULL;
    buffer->num_lines = 0;
    buffer->current_line_node = NULL;
    buffer->current_col_offset = 0;
    
    if (buffer->state) {
        free_editor_state(buffer->state);
        buffer->state = NULL;
    }
}

void saveToFile(const char *filename, TextBuffer *buffer) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        set_status_message(buffer->state, "Error: Could not open file for writing");
        return;
    }

    Line *current_line = buffer->head;
    int save_successful = 1;
    
    while (current_line != NULL && save_successful) {
        char *line_text = gap_buffer_to_string(current_line->text_buffer);
        if (line_text) {
            if (fprintf(file, "%s\n", line_text) < 0) {
                save_successful = 0;
            }
            free(line_text);
        } else {
            save_successful = 0;
        }
        current_line = current_line->next;
    }

    if (fclose(file) != 0) {
        save_successful = 0;
    }
    
    if (save_successful) {
        char success_msg[300];
        snprintf(success_msg, sizeof(success_msg), "File saved: %s", filename);
        set_status_message(buffer->state, success_msg);
    } else {
        set_status_message(buffer->state, "Error: Failed to save file completely");
    }
}

void loadFromFile(const char *filename, TextBuffer *buffer) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // File doesn't exist - create a new empty buffer with one line
        init_editor_buffer();
        Line *initial_line = create_new_line("");
        if (initial_line) {
            insert_line_at_end(buffer, initial_line);
            buffer->current_line_node = initial_line;
            buffer->current_col_offset = 0;
        }
        char msg[300];
        snprintf(msg, sizeof(msg), "New file: %s", filename);
        set_status_message(buffer->state, msg);
        return;
    }

    // Free any existing content
    free_editor_buffer(buffer);
    init_editor_buffer(); // Reinitialize after freeing

    char *line_buffer = NULL;
    size_t len = 0;
    ssize_t read;
    int lines_loaded = 0;

    while ((read = getline(&line_buffer, &len, file)) != -1) {
        // Remove the newline character if it exists
        if (read > 0 && line_buffer[read - 1] == '\n') {
            line_buffer[read - 1] = '\0';
        }
        
        Line *new_line = create_new_line(line_buffer);
        if (new_line) {
            insert_line_at_end(buffer, new_line);
            lines_loaded++;
        } else {
            set_status_message(buffer->state, "Warning: Some lines could not be loaded due to memory issues");
        }
    }

    free(line_buffer); // Free the buffer allocated by getline
    fclose(file);

    // If file was empty, create at least one empty line
    if (buffer->head == NULL) {
        Line *initial_line = create_new_line("");
        if (initial_line) {
            insert_line_at_end(buffer, initial_line);
            buffer->current_line_node = initial_line;
            buffer->current_col_offset = 0;
        }
    } else {
        // Set the cursor to the beginning of the file
        buffer->current_line_node = buffer->head;
        buffer->current_col_offset = 0;
    }
    
    char success_msg[300];
    snprintf(success_msg, sizeof(success_msg), "Loaded %d lines from %s", lines_loaded, filename);
    set_status_message(buffer->state, success_msg);
}