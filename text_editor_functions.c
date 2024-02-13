#include "text_editor_functions.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char buffer[MAX_LINES][MAX_COLS];
int current_line = 1;
int current_col = 5;
int top_line = 0;
int special_mode = 0;
int line_numbers[MAX_LINES];

void drawLineNumbers(int visible_lines) {
  for (int i = 0; i < visible_lines; ++i) {
    if (i + top_line == current_line || buffer[i + top_line][0] != '\0') {
      mvprintw(i, 1, "~");
    } else {
      mvprintw(i, 1, "-");
    }
  }
}

void drawTextContent(int visible_lines) {
    int start_line = current_line - (visible_lines / 2);
    int end_line = start_line + visible_lines;
    if (start_line < 0) {
        start_line = 0;
        end_line = visible_lines;
    } else if (end_line > MAX_LINES) {
        end_line = MAX_LINES;
        start_line = MAX_LINES - visible_lines;
    }

    for (int i = start_line; i < end_line; ++i) {
        mvprintw(i - start_line, 5, "%s", buffer[i]);
    }
}

void drawSpecialMenu(const char *command, const char *output) {
    int menu_width = 40;
    int menu_height = 5;
    int start_x = (COLS - menu_width) / 2;
    int start_y = (LINES - menu_height) / 2;

    // Draw box
    attron(A_BOLD | A_REVERSE);
    for (int y = start_y; y < start_y + menu_height; ++y) {
        mvhline(y, start_x, ' ', menu_width);
    }
    mvaddch(start_y, start_x, ACS_ULCORNER);
    mvaddch(start_y, start_x + menu_width - 1, ACS_URCORNER);
    mvaddch(start_y + menu_height - 1, start_x, ACS_LLCORNER);
    mvaddch(start_y + menu_height - 1, start_x + menu_width - 1, ACS_LRCORNER);
    mvvline(start_y, start_x, ' ', menu_height);
    mvvline(start_y, start_x + menu_width - 1, ' ', menu_height);
    attroff(A_BOLD | A_REVERSE);

    // Print command and output
    mvprintw(start_y + 1, start_x + (menu_width - strlen(command)) / 2, "%s", command);
    mvprintw(start_y + 2, start_x + (menu_width - strlen(output)) / 2, "%s", output);
}


void handleNormalModeInput(int ch) {
  switch (ch) {
  case 10: // Enter key
    if (current_line < MAX_LINES - 1) {
      // Shift lines below the current line down
      for (int i = MAX_LINES - 1; i > current_line + 1; --i) {
        strcpy(buffer[i], buffer[i - 1]);
      }
      // Insert an empty line at the current position
      buffer[current_line + 1][0] = '\0';
      // Move to the next line
      current_line++;
      current_col = 5;
    }else if (strcmp(&buffer[current_line+1][5], "\0")){
        current_line++;
        current_col = 5;
    }
    break;
  case KEY_BACKSPACE:
  case 127: // Backspace key
    if (current_col > 5) {
      current_col--;
      // Shift characters to the left
      memmove(&buffer[current_line][current_col - 5],
              &buffer[current_line][current_col + 1 - 5],
              strlen(&buffer[current_line][current_col - 5]));
      buffer[current_line][strlen(buffer[current_line])] =
          '\0'; // Null terminate the string
    } else if (current_line > 1) {
      // Shift lines below the current line up
      for (int i = current_line; i < MAX_LINES - 1 && buffer[i][0] != '\0';
           ++i) {
        strcpy(buffer[i], buffer[i + 1]);
      }
      buffer[MAX_LINES - 1][0] = '\0'; // Clear last line
      // Move cursor to end of previous line
      current_line--;
      current_col = strlen(buffer[current_line]) + 5;
    }
    break;

  case KEY_DC: // Delete key
    if (current_col < strlen(buffer[current_line]) + 5) {
      memmove(&buffer[current_line][current_col - 5],
              &buffer[current_line][current_col + 1 - 5],
              strlen(&buffer[current_line][current_col - 5]));
      buffer[current_line][strlen(buffer[current_line])] = '\0';
    } else if (current_line < MAX_LINES - 1 &&
               buffer[current_line + 1][0] != '\0') {
      // Merge current line with next line
      strcat(buffer[current_line], buffer[current_line + 1]);
      // Shift subsequent lines up
      for (int i = current_line + 1;
           i < MAX_LINES - 1 && buffer[i + 1][0] != '\0'; ++i) {
        strcpy(buffer[i], buffer[i + 1]);
      }
      buffer[MAX_LINES - 1][0] = '\0'; // Clear last line
    }
    break;
  case KEY_UP:
    if (current_line > 0) {
      current_line--;
      if (current_line < top_line)
        top_line = current_line;
    }
    break;
  case KEY_DOWN:
    if (current_line < MAX_LINES - 1 && strcmp(&buffer[current_line+1][0], "\0")) {
      current_line++;
      if (current_line >= top_line + LINES)
        top_line = current_line - LINES + 1;
    }else if (current_line < MAX_LINES - 1 && !strcmp(&buffer[current_line+1][0], "\0")){
        current_line++;
        current_col = 5;
        if (current_line >= top_line + LINES)
            top_line = current_line - LINES + 1;
    }
    break;
  case KEY_LEFT:
    if (current_col > 5) {
      current_col--;
    }
    break;
  case KEY_RIGHT:
    if (current_col < strlen(buffer[current_line]) + 5) {
      current_col++;
    }
    break;
  case 27:                           // Escape key
    special_mode = 1 - special_mode; // Toggle special mode
    break;
  default:
    if (buffer[current_line][current_col - 5] == '\0') {
      // Fill preceding characters with spaces
      for (int i = strlen(buffer[current_line]); i < current_col - 5; ++i) {
        buffer[current_line][i] = ' ';
      }
      buffer[current_line][current_col - 5 + 1] = '\0';
    }
    if (current_col >= 5) {
      buffer[current_line][current_col - 5] = ch;
      current_col++;
    }
    break;
  }
}

void handleSpecialModeInput(int ch, char *command) {
    static int command_index = 0;

    switch (ch) {
    case 10: // Enter key
        if (strcmp(command, "w") == 0) {
            int menu_width = 40;
        int menu_height = 5;
        int start_x = (COLS - menu_width) / 2;
        int start_y = (LINES - menu_height) / 2;
        
        mvprintw(start_y + 1, start_x + (menu_width - strlen("Save as: ")) / 2, "Save as: ");

        refresh();
        echo(); // Enable input echoing
        char filename[MAX_COLS];
        getstr(filename);
        noecho(); // Disable input echoing
        saveToFile(filename);
        } else if (strcmp(command, "q") == 0) {
            endwin();
            exit(EXIT_SUCCESS);
        }
        command[0] = '\0'; // Reset command buffer
        command_index = 0; // Reset command index
        special_mode = 0;  // Exit special mode
        break;
    case 27:                           // Escape key
        special_mode = 1 - special_mode; // Toggle special mode
        break;
    default:
        if (isprint(ch) && command_index < MAX_COMMAND_LENGTH - 1) {
            command[command_index++] = ch;
            command[command_index] = '\0'; // Null-terminate the command string
        }
        break;
    }
}

void handleInput(char *command) {
    int ch = getch();
    if (special_mode) {
        handleSpecialModeInput(ch, command);
    } else {
        handleNormalModeInput(ch);
    }
}