#ifndef TEXT_EDITOR_FUNCTIONS_H
#define TEXT_EDITOR_FUNCTIONS_H

#ifdef _WIN32
#include <pdcurses.h>
#else
#include <ncurses.h>
#endif

#include "data_structures.h"
#include "color_config.h"
#include "editor_state.h"
// Note: undo.h is included in the .c file to avoid circular dependencies

#define MAX_COMMAND_LENGTH 30

// File operations
void saveToFile(const char *filename, TextBuffer *buffer);
void loadFromFile(const char *filename, TextBuffer *buffer);

// Display functions
void drawLineNumbers(int visible_lines, const TextBuffer *buffer, int top_line);
void drawTextContent(int visible_lines, const TextBuffer *buffer, int top_line, int line_wrap_enabled);
void drawStatusBar(const EditorState *state, const char *command);
void drawModeIndicator(EditorMode mode, int line_wrap_enabled);

// Line wrapping functions
int get_wrapped_line_count(const char *text, int max_width, int line_wrap_enabled);
void draw_wrapped_line(int row, int col, const char *text, int max_width, int color_pair, int line_wrap_enabled);
int get_cursor_screen_row(const TextBuffer *buffer, int visible_lines, int top_line, int line_wrap_enabled);

// Input handling
void handleNormalModeInput(int ch, EditorState *state);
void handleInsertModeInput(int ch, EditorState *state);
void handleCommandModeInput(int ch, char *command, EditorState *state);
void handleInput(char *command, EditorState *state);

// Utility functions
int get_absolute_line_number(const TextBuffer *buffer, Line *target_line);
const char* get_mode_string(EditorMode mode);

#endif