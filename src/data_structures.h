#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include <stddef.h> // for size_t
#include <time.h>   // for time_t

// Editor mode definitions
typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND
} EditorMode;

// Gap buffer for efficient text editing within a line
typedef struct GapBuffer {
    char *buffer;       // The actual text buffer
    size_t capacity;    // Total allocated size
    size_t gap_start;   // Start of the gap
    size_t gap_end;     // End of the gap (exclusive)
} GapBuffer;

// Single line of text using gap buffer
typedef struct Line {
    GapBuffer *text_buffer; // Gap buffer for efficient editing
    struct Line *next;      // Pointer to the next line in the list
    struct Line *prev;      // Pointer to the previous line in the list (for a doubly linked list)
} Line;

// Editor state structure to replace globals
typedef struct EditorState {
    int top_line;                    // First visible line number
    EditorMode current_mode;         // Current editor mode
    char status_message[256];        // Status/error message
    time_t status_message_time;      // When the status message was set
    int wrap_lines;                  // Line wrapping enabled/disabled
    int tab_width;                   // Tab width for display
} EditorState;

// To manage the entire file
typedef struct TextBuffer {
    Line *head;                      // Pointer to the first line
    Line *tail;                      // Pointer to the last line
    size_t num_lines;                // Total number of lines in the buffer
    Line *current_line_node;         // Pointer to the currently active line for cursor position
    size_t current_col_offset;       // Column offset within the current line
    EditorState *state;              // Editor state
} TextBuffer;

// Constants for memory management
#define INITIAL_BUFFER_SIZE 256
#define CHUNK_SIZE 256
#define MAX_LINE_WIDTH 1000

// Global instance of the text buffer
extern TextBuffer editor_buffer;

// Gap buffer functions
GapBuffer* create_gap_buffer(size_t initial_size);
void gap_buffer_insert_char(GapBuffer *gb, char c);
void gap_buffer_delete_char(GapBuffer *gb);
void gap_buffer_move_cursor(GapBuffer *gb, size_t position);
char* gap_buffer_to_string(GapBuffer *gb);
size_t gap_buffer_length(GapBuffer *gb);
void free_gap_buffer(GapBuffer *gb);
void gap_buffer_insert_string(GapBuffer *gb, const char *str);

// Line functions
void init_editor_buffer();
Line* create_new_line(const char *content);
void insert_line_after(Line *prev_line, Line *new_line);
void free_editor_buffer(TextBuffer *buffer);
void insert_line_at_end(TextBuffer *buffer, Line *new_line);

// Editor state functions
EditorState* create_editor_state();
void free_editor_state(EditorState *state);
void set_status_message(EditorState *state, const char *message);
int should_clear_status_message(EditorState *state);

// Utility functions
size_t get_display_width(const char *text, int tab_width);
int calculate_wrapped_lines(const char *text, int max_width, int tab_width);

#endif