#include <curses.h>
#include <stdlib.h>
#include "text_editor_functions.h"
#include "data_structures.h"
#include "color_config.h"

int main(int argc, char *argv[]) {
    initscr();

    // Fix escape key delay - set to 25ms instead of default 1000ms
    set_escdelay(25);

    start_color();
    init_editor_colors();  // Initialize all color pairs from config
    cbreak();
    keypad(stdscr, TRUE);
    noecho(); // Disable input echoing

    init_editor_buffer();

    const char *filename = NULL;
    if (argc > 1) {
        filename = argv[1];
        loadFromFile(argv[1], &editor_buffer);
    } else {
        Line *initial_line = create_new_line("");
        insert_line_at_end(&editor_buffer, initial_line);
        editor_buffer.current_line_node = initial_line;
    }

    // Safety check - ensure we always have at least one line and valid cursor position
    if (editor_buffer.head == NULL) {
        Line *initial_line = create_new_line("");
        insert_line_at_end(&editor_buffer, initial_line);
        editor_buffer.current_line_node = initial_line;
    }
    if (editor_buffer.current_line_node == NULL) {
        editor_buffer.current_line_node = editor_buffer.head;
        editor_buffer.current_col_offset = 0;
    }

    int max_row, max_col;

    char command[MAX_COMMAND_LENGTH] = "";

    while (1) {
        getmaxyx(stdscr, max_row, max_col);
        int visible_lines = max_row - 1; // Reserve one line for status bar
        clear();

        attron(COLOR_PAIR(COLOR_PAIR_LINE_NUMBERS));
        drawLineNumbers(visible_lines, &editor_buffer);
        attroff(COLOR_PAIR(COLOR_PAIR_LINE_NUMBERS));
        drawTextContent(visible_lines, &editor_buffer);

        // Draw status bar at bottom
        drawStatusBar(filename, &editor_buffer, special_mode ? command : NULL);

        // Fixed cursor positioning
        int cursor_line = get_absolute_line_number(&editor_buffer, editor_buffer.current_line_node);
        int screen_row = cursor_line - top_line;

        // Ensure the cursor is visible on screen
        if (screen_row < 0) {
            top_line = cursor_line;
            screen_row = 0;
        } else if (screen_row >= visible_lines) {
            top_line = cursor_line - visible_lines + 1;
            screen_row = visible_lines - 1;
        }

        move(screen_row, editor_buffer.current_col_offset + 8);
        refresh();

        handleInput(command, &editor_buffer, filename);
    }

    endwin();
    free_editor_buffer(&editor_buffer);
    return 0;
}
