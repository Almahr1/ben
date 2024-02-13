#ifndef TEXT_EDITOR_FUNCTIONS_H
#define TEXT_EDITOR_FUNCTIONS_H

#include <ncurses.h>

#define MAX_LINES 100
#define MAX_COLS 80
#define MAX_COMMAND_LENGTH 30

extern char buffer[MAX_LINES][MAX_COLS];
extern int current_line;
extern int current_col;
extern int top_line;
extern int special_mode;
extern int line_numbers[MAX_LINES];

void saveToFile(const char *filename);
void loadFromFile(const char *filename);
void drawLineNumbers(int visible_lines);
void drawTextContent(int visible_lines);
void drawSpecialMenu(const char *command, const char *output);
void handleNormalModeInput(int ch);
void handleSpecialModeInput(int ch, char *command);
void handleInput(char *command);

#endif
