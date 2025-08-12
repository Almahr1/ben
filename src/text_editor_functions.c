#include "text_editor_functions.h"
#include "color_config.h"
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
            // Use cursor line color for the current line indicator
            attron(COLOR_PAIR(COLOR_PAIR_CURSOR_LINE));
            mvprintw(i, 5, "->");
            attroff(COLOR_PAIR(COLOR_PAIR_CURSOR_LINE));
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
        // Use main text color for content
        attron(COLOR_PAIR(COLOR_PAIR_TEXT));
        mvprintw(i, 8, "%s", current_line_node->text);
        attroff(COLOR_PAIR(COLOR_PAIR_TEXT));
        current_line_node = current_line_node->next;
    }
}

void drawStatusBar(const char *filename, const TextBuffer *buffer, const char *command) {
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);
    int status_row = max_row - 1;

    // Set background color for status bar
    attron(COLOR_PAIR(COLOR_PAIR_STATUS_BAR));

    // Clear the status bar line
    mvhline(status_row, 0, ' ', max_col);

    // Draw filename on the left
    if (filename != NULL && strlen(filename) > 0) {
        mvprintw(status_row, 1, "%s", filename);
    } else {
        mvprintw(status_row, 1, "[No Name]");
    }

    // Draw cursor position on the right
    int cursor_line = get_absolute_line_number(buffer, buffer->current_line_node) + 1; // 1-based for display
    int cursor_col = buffer->current_col_offset + 1; // 1-based for display
    char position_text[50];
    snprintf(position_text, sizeof(position_text), "Line %d, Col %d", cursor_line, cursor_col);
    int pos_len = strlen(position_text);
    mvprintw(status_row, max_col - pos_len - 1, "%s", position_text);

    // If in command mode, show command input
    if (special_mode && command != NULL) {
        // Change to command input colors
        attroff(COLOR_PAIR(COLOR_PAIR_STATUS_BAR));
        attron(COLOR_PAIR(COLOR_PAIR_COMMAND));

        // Clear a section in the middle for the command
        int command_start = 20; // Start position for command
        int command_width = max_col - command_start - pos_len - 5; // Leave space for position

        if (command_width > 0) {
            mvhline(status_row, command_start, ' ', command_width);
            mvprintw(status_row, command_start, ":%s", command);

            // Add cursor indicator after the command
            int command_cursor_pos = command_start + 1 + strlen(command);
            if (command_cursor_pos < max_col - pos_len - 2) {
                mvaddch(status_row, command_cursor_pos, ' ' | A_BLINK);
            }
        }

        attroff(COLOR_PAIR(COLOR_PAIR_COMMAND));
        attron(COLOR_PAIR(COLOR_PAIR_STATUS_BAR));
    }

    attroff(COLOR_PAIR(COLOR_PAIR_STATUS_BAR));
}

void handleNormalModeInput(int ch, TextBuffer *buffer) {
    Line *line = buffer->current_line_node;
    size_t current_col = buffer->current_col_offset;
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    // Calculate visible lines (subtract 1 for status bar)
    int visible_lines = max_row - 1;

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

    case ':': // Colon key - enter command mode
        special_mode = 1;
        break;

    case 27: // Escape key - can also toggle special mode
        special_mode = 1 - special_mode;
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

void handleSpecialModeInput(int ch, char *command, TextBuffer *buffer, const char *filename) {
    static int command_index = 0;

    switch (ch) {
    case 10: // Enter key
        if (strcmp(command, "q") == 0) {
            endwin();
            exit(EXIT_SUCCESS);
        } else if (strcmp(command, "w") == 0) {
            if (filename != NULL && strlen(filename) > 0) {
                saveToFile(filename, buffer);
                // You could add some feedback here
            }
        } else if (strncmp(command, "w ", 2) == 0) {
            // Save with specified filename: "w filename.txt"
            const char *save_filename = command + 2; // Skip "w "
            if (strlen(save_filename) > 0) {
                saveToFile(save_filename, buffer);
            }
        } else if (strcmp(command, "wq") == 0) {
            if (filename != NULL && strlen(filename) > 0) {
                saveToFile(filename, buffer);
            }
            endwin();
            exit(EXIT_SUCCESS);
        } else if (strncmp(command, "wq ", 3) == 0) {
            // Save with specified filename and quit: "wq filename.txt"
            const char *save_filename = command + 3; // Skip "wq "
            if (strlen(save_filename) > 0) {
                saveToFile(save_filename, buffer);
            }
            endwin();
            exit(EXIT_SUCCESS);
        }

        command[0] = '\0'; // Reset command buffer
        command_index = 0; // Reset command index
        special_mode = 0;  // Exit special mode
        break;
    case 27: // Escape key
        command[0] = '\0'; // Reset command buffer
        command_index = 0; // Reset command index
        special_mode = 0; // Exit special mode
        break;
    case KEY_BACKSPACE:
    case 127: // Backspace in command mode
        if (command_index > 0) {
            command_index--;
            command[command_index] = '\0';
        }
        break;
    default:
        if (isprint(ch) && command_index < MAX_COMMAND_LENGTH - 1) {
            command[command_index++] = ch;
            command[command_index] = '\0'; // Null-terminate the command string
        }
        break;
    }
}

void handleInput(char *command, TextBuffer *buffer, const char *filename) {
    int ch = getch();
    if (special_mode) {
        handleSpecialModeInput(ch, command, buffer, filename);
    } else {
        handleNormalModeInput(ch, buffer);
    }
}
