#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#include <stddef.h>

typedef struct {
    char *buffer;           // The actual character buffer
    size_t capacity;        // Total size of the buffer
    size_t gap_start;       // Start position of the gap
    size_t gap_end;         // End position of the gap (exclusive)
} GapBuffer;

// Gap buffer operations
GapBuffer* gap_buffer_create(size_t initial_capacity);
void gap_buffer_destroy(GapBuffer *gb);

// Cursor movement
void gap_buffer_move_cursor_to(GapBuffer *gb, size_t position);
size_t gap_buffer_cursor_position(const GapBuffer *gb);

// Text operations
void gap_buffer_insert_char(GapBuffer *gb, char c);
void gap_buffer_insert_string(GapBuffer *gb, const char *str);
void gap_buffer_delete_char(GapBuffer *gb);
void gap_buffer_delete_char_before(GapBuffer *gb);

// Query operations
size_t gap_buffer_length(const GapBuffer *gb);
char gap_buffer_get_char_at(const GapBuffer *gb, size_t position);
char* gap_buffer_to_string(const GapBuffer *gb);

// Utility functions
void gap_buffer_ensure_capacity(GapBuffer *gb, size_t needed_capacity);
size_t gap_buffer_gap_size(const GapBuffer *gb);

// Debug function
void gap_buffer_print_debug(const GapBuffer *gb);

#endif