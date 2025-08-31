#ifdef _WIN32
#include <pdcurses.h>
#else
#include <curses.h>
#endif

#include "color_config.h"
#include "data_structures.h"
#include "editor_state.h"
#include "text_editor_functions.h"
#include "undo.h"

int
main (int argc, char *argv[])
{
  initscr ();

  set_escdelay (25);

  start_color ();
  init_editor_colors ();
  cbreak ();
  keypad (stdscr, TRUE);
  noecho ();

  EditorState editor_state;
  const char *filename = (argc > 1) ? argv[1] : NULL;
  init_editor_state (&editor_state, filename);

  init_undo_system ();

  char command[MAX_COMMAND_LENGTH] = "";

  while (1)
    {
      int max_row = getmaxy (stdscr);
      int max_col = getmaxx (stdscr);
      int visible_lines = max_row - 2;
      int text_width = max_col - 8;
      clear ();

      drawModeIndicator (editor_state.current_mode,
                         editor_state.line_wrap_enabled);

      attron (COLOR_PAIR (COLOR_PAIR_LINE_NUMBERS));
      drawLineNumbers (visible_lines, &editor_state.buffer,
                       editor_state.top_line);
      attroff (COLOR_PAIR (COLOR_PAIR_LINE_NUMBERS));
      drawTextContent (visible_lines, &editor_state.buffer,
                       editor_state.top_line, editor_state.line_wrap_enabled);

      drawStatusBar (&editor_state, editor_state.current_mode == MODE_COMMAND
                                        ? command
                                        : NULL);

      int cursor_screen_row = get_cursor_screen_row (
          &editor_state.buffer, visible_lines, editor_state.top_line,
          editor_state.line_wrap_enabled);
      int cursor_screen_col;

      if (editor_state.line_wrap_enabled
          && editor_state.buffer.current_line_node != NULL)
        {
          cursor_screen_col
              = 8 + (editor_state.buffer.current_col_offset % text_width);
        }
      else
        {
          cursor_screen_col = editor_state.buffer.current_col_offset + 8;
        }

      if (cursor_screen_row < 1)
        {
          editor_state.top_line -= (1 - cursor_screen_row);
          if (editor_state.top_line < 0)
            editor_state.top_line = 0;
          cursor_screen_row = 1;
        }
      else if (cursor_screen_row > visible_lines)
        {
          editor_state.top_line += (cursor_screen_row - visible_lines);
          cursor_screen_row = visible_lines;
        }

      move (cursor_screen_row, cursor_screen_col);
      refresh ();

      handleInput (command, &editor_state);
    }

  endwin ();
  free_editor_state (&editor_state);
  return 0;
}
