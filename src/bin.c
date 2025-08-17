#ifdef _WIN32
#include <pdcurses.h>
#else
#include <curses.h>
#endif

#include <stdlib.h>
#include "text_editor_functions.h"
#include "data_structures.h"
#include "color_config.h"
#include "editor_state.h"
#include "undo.h"

int main(int argc, char *argv[]) {
    initscr();

    // Fix escape key delay - set to 25ms instead of default 1000ms
    set_escdelay(25);

    start_color();
    init_editor_colors();  // Initialize all color pairs from config
    cbreak();
    keypad(stdscr, TRUE);
    noecho(); // Disable input echoing

    // Initialize editor state
    EditorState editor_state;
    const char *filename = (argc > 1) ? argv[1] : NULL;
    init_editor_state(&editor_state, filename);
    
    // Initialize undo system
    init_undo_system();

    char command[MAX_COMMAND_LENGTH] = "";

    while (1) {
        int max_row = getmaxy(stdscr);
        int max_col = getmaxx(stdscr);
        int visible_lines = max_row - 2; // Reserve one line for status bar and one for mode indicator
        int text_width = max_col - 8; // Available width for text content
        clear();

        // Draw mode indicator first (top-left corner)
        drawModeIndicator(editor_state.current_mode, editor_state.line_wrap_enabled);

        attron(COLOR_PAIR(COLOR_PAIR_LINE_NUMBERS));
        drawLineNumbers(visible_lines, &editor_state.buffer, editor_state.top_line);
        attroff(COLOR_PAIR(COLOR_PAIR_LINE_NUMBERS));
        drawTextContent(visible_lines, &editor_state.buffer, editor_state.top_line, editor_state.line_wrap_enabled);

        // Draw status bar at bottom
        drawStatusBar(&editor_state, editor_state.current_mode == MODE_COMMAND ? command : NULL);

        // Calculate cursor position with line wrapping support
        int cursor_screen_row = get_cursor_screen_row(&editor_state.buffer, visible_lines, editor_state.top_line, editor_state.line_wrap_enabled);
        int cursor_screen_col;
        
        if (editor_state.line_wrap_enabled && editor_state.buffer.current_line_node != NULL) {
            // Calculate column position within the wrapped line
            cursor_screen_col = 8 + (editor_state.buffer.current_col_offset % text_width);
        } else {
            cursor_screen_col = editor_state.buffer.current_col_offset + 8;
        }

        // Ensure the cursor is visible on screen
        if (cursor_screen_row < 1) { // Must be at least row 1 (below mode indicator)
            editor_state.top_line -= (1 - cursor_screen_row);
            if (editor_state.top_line < 0) editor_state.top_line = 0;
            cursor_screen_row = 1;
        } else if (cursor_screen_row > visible_lines) {
            editor_state.top_line += (cursor_screen_row - visible_lines);
            cursor_screen_row = visible_lines;
        }

        move(cursor_screen_row, cursor_screen_col);
        refresh();

        handleInput(command, &editor_state);
    }

    endwin();
    free_editor_state(&editor_state);
    return 0;
}