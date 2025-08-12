#ifndef TEXT_EDITOR_FUNCTIONS_H
#define TEXT_EDITOR_FUNCTIONS_H

#include <ncurses.h>
#include "data_structures.h" // Include the new header
#include "color_config.h"    // Include color configuration

#define MAX_COMMAND_LENGTH 30

// Editor mode definitions
typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND
} EditorMode;

extern int top_line;
extern EditorMode current_mode;

// File operations
void saveToFile(const char *filename, TextBuffer *buffer);
void loadFromFile(const char *filename, TextBuffer *buffer);

// Display functions
void drawLineNumbers(int visible_lines, const TextBuffer *buffer);
void drawTextContent(int visible_lines, const TextBuffer *buffer);
void drawStatusBar(const char *filename, const TextBuffer *buffer, const char *command);
void drawModeIndicator(EditorMode mode);

// Input handling
void handleNormalModeInput(int ch, TextBuffer *buffer);
void handleInsertModeInput(int ch, TextBuffer *buffer);
void handleCommandModeInput(int ch, char *command, TextBuffer *buffer, const char *filename);
void handleInput(char *command, TextBuffer *buffer, const char *filename);

// Utility functions
int get_absolute_line_number(TextBuffer *buffer, Line *target_line);
const char* get_mode_string(EditorMode mode);

#endif