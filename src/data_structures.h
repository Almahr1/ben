#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h> // for size_t
#include "gap_buffer.h"

// Single line of text using gap buffer for efficient editing
typedef struct Line {
    GapBuffer *gb;          // Gap buffer for this line's content
    struct Line *next;      // Pointer to the next line in the list
    struct Line *prev;      // Pointer to the previous line in the list (for a doubly linked list)
} Line;

// To manage the entire file, you might have a head and tail pointer
// and potentially a count of lines.
typedef struct TextBuffer {
    Line *head;                 // Pointer to the first line
    Line *tail;                 // Pointer to the last line
    size_t num_lines;           // Total number of lines in the buffer
    Line *current_line_node;    // Pointer to the currently active line for cursor position
    size_t current_col_offset;  // Column offset within the current line
} TextBuffer;

// Global instance of the text buffer
extern TextBuffer editor_buffer;

// Function declarations
void init_editor_buffer(TextBuffer *buffer);
Line* create_new_line(const char *content);
Line* create_new_line_empty();
void insert_line_after(Line *prev_line, Line *new_line);
void free_editor_buffer(TextBuffer *buffer);
void insert_line_at_end(TextBuffer *buffer, Line *new_line);

// New gap buffer helper functions for line operations
size_t line_get_length(const Line *line);
char line_get_char_at(const Line *line, size_t position);
char* line_to_string(const Line *line);
void line_insert_char_at(Line *line, size_t position, char c);
void line_insert_string_at(Line *line, size_t position, const char *str);
void line_delete_char_at(Line *line, size_t position);
void line_delete_char_before(Line *line, size_t position);

#endif