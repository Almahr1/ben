#ifndef TEXT_EDITOR_FUNCTIONS_H
#define TEXT_EDITOR_FUNCTIONS_H

#include <ncurses.h>
#include "data_structures.h" // Include the new header
#include "color_config.h"    // Include color configuration

#define MAX_COMMAND_LENGTH 30

extern int top_line;
extern int special_mode;

// File operations
void saveToFile(const char *filename, TextBuffer *buffer);
void loadFromFile(const char *filename, TextBuffer *buffer);

// Display functions
void drawLineNumbers(int visible_lines, const TextBuffer *buffer);
void drawTextContent(int visible_lines, const TextBuffer *buffer);
void drawStatusBar(const char *filename, const TextBuffer *buffer, const char *command);

// Input handling
void handleNormalModeInput(int ch, TextBuffer *buffer);
void handleSpecialModeInput(int ch, char *command, TextBuffer *buffer, const char *filename);
void handleInput(char *command, TextBuffer *buffer, const char *filename);

// Utility functions
int get_absolute_line_number(TextBuffer *buffer, Line *target_line);

#endif
