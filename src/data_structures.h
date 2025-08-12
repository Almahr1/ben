#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h> // for size_t

// Single line of text
typedef struct Line {
    char *text; // Pointer to dynamically allocated memory for the line's content
    size_t length; // Current length of the line's content
    size_t capacity; // Allocated size of the 'text' buffer
    struct Line *next; // Pointer to the next line in the list
    struct Line *prev; // Pointer to the previous line in the list (for a doubly linked list)
} Line;

// To manage the entire file, you might have a head and tail pointer
// and potentially a count of lines.
typedef struct TextBuffer {
    Line *head; // Pointer to the first line
    Line *tail; // Pointer to the last line
    size_t num_lines; // Total number of lines in the buffer
    Line *current_line_node; // Pointer to the currently active line for cursor position
    size_t current_col_offset; // Column offset within the current line
} TextBuffer;

// Global instance of the text buffer
extern TextBuffer editor_buffer;

void init_editor_buffer();
Line* create_new_line(const char *content);
void insert_line_after(Line *prev_line, Line *new_line);
void free_editor_buffer(TextBuffer *buffer);
void insert_line_at_end(TextBuffer *buffer, Line *new_line);

#endif
