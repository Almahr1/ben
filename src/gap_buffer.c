#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "data_structures.h"

// Gap buffer implementation for efficient text editing
GapBuffer* create_gap_buffer(size_t initial_size) {
    GapBuffer *gb = malloc(sizeof(GapBuffer));
    if (!gb) return NULL;
    
    gb->buffer = malloc(initial_size);
    if (!gb->buffer) {
        free(gb);
        return NULL;
    }
    
    gb->capacity = initial_size;
    gb->gap_start = 0;
    gb->gap_end = initial_size;
    
    return gb;
}

static void expand_gap_buffer(GapBuffer *gb) {
    size_t new_capacity = gb->capacity + CHUNK_SIZE;
    char *new_buffer = malloc(new_capacity);
    if (!new_buffer) return; // Handle gracefully
    
    // Copy text before gap
    memcpy(new_buffer, gb->buffer, gb->gap_start);
    
    // Copy text after gap, leaving more gap space
    size_t text_after_gap = gb->capacity - gb->gap_end;
    size_t new_gap_end = new_capacity - text_after_gap;
    
    memcpy(new_buffer + new_gap_end, gb->buffer + gb->gap_end, text_after_gap);
    
    free(gb->buffer);
    gb->buffer = new_buffer;
    gb->capacity = new_capacity;
    gb->gap_end = new_gap_end;
}

void gap_buffer_move_cursor(GapBuffer *gb, size_t position) {
    if (position <= gb->gap_start) {
        // Move gap right
        size_t move_count = gb->gap_start - position;
        memmove(gb->buffer + gb->gap_end - move_count, 
                gb->buffer + position, move_count);
        gb->gap_start = position;
        gb->gap_end -= move_count;
    } else {
        // Move gap left
        size_t text_length = gb->capacity - (gb->gap_end - gb->gap_start);
        if (position > text_length) position = text_length;
        
        size_t move_count = position - gb->gap_start;
        memmove(gb->buffer + gb->gap_start,
                gb->buffer + gb->gap_end, move_count);
        gb->gap_start += move_count;
        gb->gap_end += move_count;
    }
}

void gap_buffer_insert_char(GapBuffer *gb, char c) {
    if (gb->gap_start >= gb->gap_end) {
        expand_gap_buffer(gb);
    }
    
    gb->buffer[gb->gap_start++] = c;
}

void gap_buffer_insert_string(GapBuffer *gb, const char *str) {
    size_t len = strlen(str);
    
    // Ensure we have enough space
    while (gb->gap_end - gb->gap_start < len) {
        expand_gap_buffer(gb);
    }
    
    // Insert each character
    for (size_t i = 0; i < len; i++) {
        gb->buffer[gb->gap_start++] = str[i];
    }
}

void gap_buffer_delete_char(GapBuffer *gb) {
    if (gb->gap_start > 0) {
        gb->gap_start--;
    }
}

char* gap_buffer_to_string(GapBuffer *gb) {
    size_t text_length = gb->capacity - (gb->gap_end - gb->gap_start);
    char *result = malloc(text_length + 1);
    if (!result) return NULL;
    
    // Copy text before gap
    memcpy(result, gb->buffer, gb->gap_start);
    
    // Copy text after gap
    size_t text_after_gap = gb->capacity - gb->gap_end;
    memcpy(result + gb->gap_start, gb->buffer + gb->gap_end, text_after_gap);
    
    result[text_length] = '\0';
    return result;
}

size_t gap_buffer_length(GapBuffer *gb) {
    return gb->capacity - (gb->gap_end - gb->gap_start);
}

void free_gap_buffer(GapBuffer *gb) {
    if (gb) {
        free(gb->buffer);
        free(gb);
    }
}

// Editor state functions
EditorState* create_editor_state() {
    EditorState *state = malloc(sizeof(EditorState));
    if (!state) return NULL;
    
    state->top_line = 0;
    state->current_mode = MODE_NORMAL;
    state->status_message[0] = '\0';
    state->status_message_time = 0;
    state->wrap_lines = 1; // Enable line wrapping by default
    state->tab_width = 4;  // Default tab width
    
    return state;
}

void free_editor_state(EditorState *state) {
    if (state) {
        free(state);
    }
}

void set_status_message(EditorState *state, const char *message) {
    strncpy(state->status_message, message, sizeof(state->status_message) - 1);
    state->status_message[sizeof(state->status_message) - 1] = '\0';
    state->status_message_time = time(NULL);
}

int should_clear_status_message(EditorState *state) {
    if (state->status_message[0] == '\0') return 0;
    return (time(NULL) - state->status_message_time) > 3; // Clear after 3 seconds
}

// Utility functions
size_t get_display_width(const char *text, int tab_width) {
    size_t width = 0;
    for (const char *p = text; *p; p++) {
        if (*p == '\t') {
            width = ((width / tab_width) + 1) * tab_width;
        } else {
            width++;
        }
    }
    return width;
}

int calculate_wrapped_lines(const char *text, int max_width, int tab_width) {
    if (max_width <= 0) return 1;
    
    int lines = 1;
    int current_width = 0;
    
    for (const char *p = text; *p; p++) {
        if (*p == '\t') {
            int next_tab_stop = ((current_width / tab_width) + 1) * tab_width;
            if (next_tab_stop >= max_width && current_width > 0) {
                lines++;
                current_width = tab_width;
            } else {
                current_width = next_tab_stop;
            }
        } else {
            current_width++;
            if (current_width >= max_width) {
                lines++;
                current_width = 0;
            }
        }
    }
    
    return lines;
}