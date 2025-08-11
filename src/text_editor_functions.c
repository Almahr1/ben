#include "text_editor_functions.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

//...

// Globals used for special mode and top_line
int top_line = 0;
int special_mode = 0;

// Helper function to get absolute line number
int get_absolute_line_number(TextBuffer *buffer, Line *target_line) {
    int line_num = 0;
    Line *current = buffer->head;

    while (current != NULL && current != target_line) {
        line_num++;
        current = current->next;
    }

    return line_num;
}

void drawLineNumbers(int visible_lines, const TextBuffer *buffer) {
    Line *current_line_node = buffer->head;
    int line_num = 1;

    // Move to the first visible line
    for (int i = 0; i < top_line && current_line_node != NULL; ++i) {
        current_line_node = current_line_node->next;
        line_num++;
    }

    for (int i = 0; i < visible_lines && current_line_node != NULL; ++i) {
        mvprintw(i, 1, "%4d", line_num);
        if (current_line_node == buffer->current_line_node) {
            mvprintw(i, 5, "->"); // Indicator for the current line
        } else {
            mvprintw(i, 5, "  ");
        }
        current_line_node = current_line_node->next;
        line_num++;
    }
}

void drawTextContent(int visible_lines, const TextBuffer *buffer) {
    Line *current_line_node = buffer->head;

    // Move to the first visible line
    for (int i = 0; i < top_line && current_line_node != NULL; ++i) {
        current_line_node = current_line_node->next;
    }

    for (int i = 0; i < visible_lines && current_line_node != NULL; ++i) {
        mvprintw(i, 8, "%s", current_line_node->text);
        current_line_node = current_line_node->next;
    }
}

// Corrected: Removed `command` parameter
void drawSpecialMenu(const char *output) {
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

    // Print output
    mvprintw(start_y + 2, start_x + (menu_width - strlen(output)) / 2, "%s", output);
}

void handleNormalModeInput(int ch, TextBuffer *buffer) {
    Line *line = buffer->current_line_node;
    size_t current_col = buffer->current_col_offset;
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col); // Fixed: separate variables for row and col

    // Calculate visible lines (accounting for potential status line)
    int visible_lines = max_row;
    if (special_mode) {
        visible_lines = max_row; // Menu doesn't reduce visible area since it's overlaid
    }

    switch (ch) {
    case 10: // Enter key
        if (line != NULL) {
            Line *new_line = create_new_line(line->text + current_col);
            // Truncate the current line
            line->text[current_col] = '\0';
            line->length = current_col;

            insert_line_after(line, new_line);
            buffer->current_line_node = new_line;
            buffer->current_col_offset = 0;

            // Fixed scrolling: check if cursor moved below visible area
            int cursor_line = get_absolute_line_number(buffer, buffer->current_line_node);
            if (cursor_line >= top_line + visible_lines) {
                top_line = cursor_line - visible_lines + 1;
            }
        }
        break;

    case KEY_BACKSPACE:
    case 127: // Backspace key
        if (current_col > 0) {
            memmove(&line->text[current_col - 1], &line->text[current_col],
                   line->length - current_col + 1);
            line->length--;
            line->text = realloc(line->text, line->length + 1);
            buffer->current_col_offset--;
        } else if (line->prev != NULL) {
            Line *prev_line = line->prev;
            size_t prev_len = prev_line->length;

            // Merge lines
            prev_line->text = realloc(prev_line->text, prev_len + line->length + 1);
            strcat(prev_line->text, line->text);
            prev_line->length = prev_len + line->length;

            // Unlink and free current line
            prev_line->next = line->next;
            if (line->next != NULL) {
                line->next->prev = prev_line;
            } else {
                buffer->tail = prev_line;
            }
            free(line->text);
            free(line);
            buffer->num_lines--;

            buffer->current_line_node = prev_line;
            buffer->current_col_offset = prev_len;

            // Fixed scrolling after merge
            int cursor_line = get_absolute_line_number(buffer, buffer->current_line_node);
            if (cursor_line < top_line) {
                top_line = cursor_line;
            }
        }
        break;

    case KEY_DC: // Delete key
        if (current_col < line->length) {
            memmove(&line->text[current_col], &line->text[current_col + 1],
                   line->length - current_col);
            line->length--;
            line->text = realloc(line->text, line->length + 1);
        } else if (line->next != NULL) {
            Line *next_line = line->next;
            size_t line_len = line->length;

            // Merge lines
            line->text = realloc(line->text, line_len + next_line->length + 1);
            strcat(line->text, next_line->text);
            line->length = line_len + next_line->length;

            // Unlink and free next line
            line->next = next_line->next;
            if (next_line->next != NULL) {
                next_line->next->prev = line;
            } else {
                buffer->tail = line;
            }
            free(next_line->text);
            free(next_line);
            buffer->num_lines--;
        }
        break;

    case KEY_UP:
        if (line->prev != NULL) {
            buffer->current_line_node = line->prev;
            if (buffer->current_col_offset > buffer->current_line_node->length) {
                buffer->current_col_offset = buffer->current_line_node->length;
            }

            // Fixed scrolling: scroll up if cursor moves above visible area
            int cursor_line = get_absolute_line_number(buffer, buffer->current_line_node);
            if (cursor_line < top_line) {
                top_line = cursor_line;
            }
        }
        break;

    case KEY_DOWN:
        if (line->next != NULL) {
            buffer->current_line_node = line->next;
            if (buffer->current_col_offset > buffer->current_line_node->length) {
                buffer->current_col_offset = buffer->current_line_node->length;
            }

            // Fixed scrolling: scroll down if cursor moves below visible area
            int cursor_line = get_absolute_line_number(buffer, buffer->current_line_node);
            if (cursor_line >= top_line + visible_lines) {
                top_line = cursor_line - visible_lines + 1;
            }
        }
        break;

    case KEY_LEFT:
        if (buffer->current_col_offset > 0) {
            buffer->current_col_offset--;
        }
        break;

    case KEY_RIGHT:
        if (buffer->current_col_offset < line->length) {
            buffer->current_col_offset++;
        }
        break;

    case 27: // Escape key
        special_mode = 1 - special_mode; // Toggle special mode
        break;

    default:
        if (isprint(ch)) {
            if (line->length + 1 >= line->capacity) {
                line->capacity *= 2;
                line->text = realloc(line->text, line->capacity);
            }
            memmove(&line->text[current_col + 1], &line->text[current_col],
                   line->length - current_col + 1);
            line->text[current_col] = ch;
            line->length++;
            buffer->current_col_offset++;
        }
        break;
    }
}

void handleSpecialModeInput(int ch, char *command) {
    static int command_index = 0;

    switch (ch) {
    case 10: // Enter key
        if (strcmp(command, "q") == 0) {
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

void handleInput(char *command, TextBuffer *buffer) {
    int ch = getch();
    if (special_mode) {
        handleSpecialModeInput(ch, command);
    } else {
        handleNormalModeInput(ch, buffer);
    }
}
