#ifndef COLOR_CONFIG_H
#define COLOR_CONFIG_H

#ifdef _WIN32
#include <pdcurses.h>
#else
#include <ncurses.h>
#endif

// Color Configuration - Change these values to customize the editor appearance
// Available colors: COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW, 
//                  COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE

#define TEXT_FG_COLOR           COLOR_CYAN
#define TEXT_BG_COLOR           COLOR_BLACK
#define LINE_NUMBER_FG_COLOR    COLOR_BLUE
#define LINE_NUMBER_BG_COLOR    COLOR_BLACK
#define CURSOR_LINE_FG_COLOR    COLOR_WHITE
#define CURSOR_LINE_BG_COLOR    COLOR_BLACK
#define STATUS_BAR_FG_COLOR     COLOR_WHITE
#define STATUS_BAR_BG_COLOR     COLOR_BLUE
#define COMMAND_INPUT_FG_COLOR  COLOR_CYAN
#define COMMAND_INPUT_BG_COLOR  COLOR_BLACK

#define NORMAL_MODE_FG_COLOR    COLOR_BLACK
#define NORMAL_MODE_BG_COLOR    COLOR_GREEN
#define INSERT_MODE_FG_COLOR    COLOR_WHITE
#define INSERT_MODE_BG_COLOR    COLOR_RED
#define COMMAND_MODE_FG_COLOR   COLOR_WHITE
#define COMMAND_MODE_BG_COLOR   COLOR_YELLOW

// Color Pair Definitions (don't change these numbers, they're used internally)
#define COLOR_PAIR_TEXT         1
#define COLOR_PAIR_LINE_NUMBERS 2
#define COLOR_PAIR_STATUS_BAR   3
#define COLOR_PAIR_COMMAND      4
#define COLOR_PAIR_CURSOR_LINE  5
#define COLOR_PAIR_NORMAL_MODE  6
#define COLOR_PAIR_INSERT_MODE  7
#define COLOR_PAIR_COMMAND_MODE 8

// Function to initialize all color pairs
void init_editor_colors(void);

#endif
