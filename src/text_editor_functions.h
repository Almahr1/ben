#ifndef TEXT_EDITOR_FUNCTIONS_H
#define TEXT_EDITOR_FUNCTIONS_H

#ifdef _WIN32
#include <pdcurses.h>
#else
#include <ncurses.h>
#endif

#include "data_structures.h" // Include the new header
#include "color_config.h"    // Include color configuration
#include "undo.h" // include undo system

#define MAX_COMMAND_LENGTH 30

// Editor mode definitions
typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND
} EditorMode;

extern int top_line;
extern EditorMode current_mode;
extern int line_wrap_enabled; // Global flag for line wrapping

// File operations
void saveToFile(const char *filename, TextBuffer *buffer);
void loadFromFile(const char *filename, TextBuffer *buffer);

// Display functions
void drawLineNumbers(int visible_lines, const TextBuffer *buffer);
void drawTextContent(int visible_lines, const TextBuffer *buffer);
void drawStatusBar(const char *filename, const TextBuffer *buffer, const char *command);
void drawModeIndicator(EditorMode mode);

// Line wrapping functions
int get_wrapped_line_count(const char *text, int max_width);
void draw_wrapped_line(int row, int col, const char *text, int max_width, int color_pair);
int get_cursor_screen_row(TextBuffer *buffer, int visible_lines);

// Temporary message functions
void set_temp_message(const char *message);
void clear_temp_message(void);
int has_temp_message(void);

// Input handling
void handleNormalModeInput(int ch, TextBuffer *buffer);
void handleInsertModeInput(int ch, TextBuffer *buffer);
void handleCommandModeInput(int ch, char *command, TextBuffer *buffer, const char *filename);
void handleInput(char *command, TextBuffer *buffer, const char *filename);

// Utility functions
int get_absolute_line_number(TextBuffer *buffer, Line *target_line);
const char* get_mode_string(EditorMode mode);

#endif