#include "editor_state.h"
#include "text_editor_functions.h"
#include <string.h>
#include <stdlib.h>

void init_editor_state(EditorState *state, const char *filename) {
    if (!state) return;
    
    // Initialize the text buffer
    init_editor_buffer(&state->buffer);
    
    // Set initial state
    state->current_mode = MODE_NORMAL;
    state->top_line = 0;
    state->line_wrap_enabled = 1;  // Enable line wrapping by default
    state->temp_message[0] = '\0'; // Clear temp message
    state->filename = filename;    // Store filename (can be NULL)
    
    // Load file or create empty buffer
    if (filename) {
        loadFromFile(filename, &state->buffer);
    } else {
        Line *initial_line = create_new_line("");
        insert_line_at_end(&state->buffer, initial_line);
        state->buffer.current_line_node = initial_line;
    }
    
    // Safety check - ensure we always have at least one line and valid cursor position
    if (state->buffer.head == NULL) {
        Line *initial_line = create_new_line("");
        insert_line_at_end(&state->buffer, initial_line);
        state->buffer.current_line_node = initial_line;
    }
    if (state->buffer.current_line_node == NULL) {
        state->buffer.current_line_node = state->buffer.head;
        state->buffer.current_col_offset = 0;
    }
}

void free_editor_state(EditorState *state) {
    if (!state) return;
    
    free_editor_buffer(&state->buffer);
    // Note: filename is not owned by EditorState, so we don't free it
}

void set_temp_message(EditorState *state, const char *message) {
    if (!state || !message) return;
    
    strncpy(state->temp_message, message, sizeof(state->temp_message) - 1);
    state->temp_message[sizeof(state->temp_message) - 1] = '\0';
}

void clear_temp_message(EditorState *state) {
    if (!state) return;
    
    state->temp_message[0] = '\0';
}

int has_temp_message(const EditorState *state) {
    if (!state) return 0;
    
    return state->temp_message[0] != '\0';
}