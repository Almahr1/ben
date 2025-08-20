#ifndef EDITOR_STATE_H
#define EDITOR_STATE_H

#include "data_structures.h"

// Editor mode definitions
typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND,
} EditorMode;

// Centralized editor state structure
typedef struct {
    TextBuffer buffer;              // The text buffer containing all lines
    EditorMode current_mode;        // Current editing mode
    int top_line;                   // First visible line on screen
    int line_wrap_enabled;          // Line wrapping toggle
    char temp_message[256];         // Temporary status messages
    const char *filename;           // Current filename (can be NULL)
} EditorState;

// EditorState management functions
void init_editor_state(EditorState *state, const char *filename);
void free_editor_state(EditorState *state);

// Temporary message functions
void set_temp_message(EditorState *state, const char *message);
void clear_temp_message(EditorState *state);
int has_temp_message(const EditorState *state);

#endif