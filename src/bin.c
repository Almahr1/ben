#include <curses.h>
#include <stdlib.h>
#include "text_editor_functions.h"

int main(int argc, char *argv[]) {
    initscr(); // Initialize ncurses
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    cbreak();             // Line buffering disabled
    keypad(stdscr, TRUE); // Enable special keys

    if (argc > 1) {
        loadFromFile(argv[1]);
    }

    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    char command[MAX_COLS] = "";
    char output[MAX_COLS] = "";
    while (1) {
        int visible_lines = max_row;
        clear();

        attron(COLOR_PAIR(1));
        drawLineNumbers(visible_lines);
        attroff(COLOR_PAIR(1));
        drawTextContent(visible_lines);

        if (special_mode) {
            drawSpecialMenu(command, output);
        }

        move(current_line - top_line, current_col);
        refresh();

        handleInput(command);
    }

    endwin(); // Clean up and close ncurses
    return 0;
}
