#ifdef _WIN32
#include <pdcurses.h>
#else
#include <ncurses.h>
#endif

#include "text_editor_functions.h"
#include "color_config.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// Globals used for mode and top_line
int top_line = 0;
EditorMode current_mode = MODE_NORMAL;
int line_wrap_enabled = 1; // Enable line wrapping by default

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

const char* get_mode_string(EditorMode mode) {
    switch (mode) {
        case MODE_NORMAL:
            return "NORMAL";
        case MODE_INSERT:
            return "INSERT";
        case MODE_COMMAND:
            return "COMMAND";
        default:
            return "UNKNOWN";
    }
}

void drawModeIndicator(EditorMode mode) {
    int color_pair;
    const char* mode_text = get_mode_string(mode);
    
    // Select appropriate color pair for the mode
    switch (mode) {
        case MODE_NORMAL:
            color_pair = COLOR_PAIR_NORMAL_MODE;
            break;
        case MODE_INSERT:
            color_pair = COLOR_PAIR_INSERT_MODE;
            break;
        case MODE_COMMAND:
            color_pair = COLOR_PAIR_COMMAND_MODE;
            break;
        default:
            color_pair = COLOR_PAIR_NORMAL_MODE;
            break;
    }
    
    // Draw the mode indicator rectangle in top-left corner
    attron(COLOR_PAIR(color_pair));
    mvprintw(0, 0, " %s ", mode_text);
    attroff(COLOR_PAIR(color_pair));
    
    // Add wrap indicator
    if (line_wrap_enabled) {
        mvprintw(0, strlen(mode_text) + 3, " [WRAP] ");
    }
}

// Calculate how many screen lines a text line will take when wrapped
int get_wrapped_line_count(const char *text, int max_width) {
    if (!line_wrap_enabled) {
        return 1;
    }
    
    int len = strlen(text);
    if (len == 0) {
        return 1;
    }
    
    return (len + max_width - 1) / max_width; // Ceiling division
}

// Draw a line with wrapping support
void draw_wrapped_line(int row, int col, const char *text, int max_width, int color_pair) {
    if (!line_wrap_enabled) {
        attron(COLOR_PAIR(color_pair));
        mvprintw(row, col, "%.*s", max_width, text);
        attroff(COLOR_PAIR(color_pair));
        return;
    }
    
    int len = strlen(text);
    int current_row = row;
    int pos = 0;
    
    attron(COLOR_PAIR(color_pair));
    while (pos < len) {
        int chars_to_print = (len - pos > max_width) ? max_width : (len - pos);
        mvprintw(current_row, col, "%.*s", chars_to_print, text + pos);
        pos += chars_to_print;
        current_row++;
    }
    attroff(COLOR_PAIR(color_pair));
}

void drawLineNumbers(int visible_lines, const TextBuffer *buffer) {
    Line *current_line_node = buffer->head;
    int line_num = 1;
    int screen_row = 1; // Start from row 1 to leave space for mode indicator
    int max_col = getmaxx(stdscr);
    int text_width = max_col - 8; // Available width for text content

    // Move to the first visible line
    for (int i = 0; i < top_line && current_line_node != NULL; ++i) {
        current_line_node = current_line_node->next;
        line_num++;
    }

    while (screen_row <= visible_lines && current_line_node != NULL) {
        int wrapped_lines = get_wrapped_line_count(current_line_node->text, text_width);
        
        // Draw line number for the first wrapped line
        attron(COLOR_PAIR(COLOR_PAIR_LINE_NUMBERS));
        mvprintw(screen_row, 1, "%4d", line_num);
        
        if (current_line_node == buffer->current_line_node) {
            // Use cursor line color for the current line indicator
            attroff(COLOR_PAIR(COLOR_PAIR_LINE_NUMBERS));
            attron(COLOR_PAIR(COLOR_PAIR_CURSOR_LINE));
            mvprintw(screen_row, 5, "->");
            attroff(COLOR_PAIR(COLOR_PAIR_CURSOR_LINE));
        } else {
            mvprintw(screen_row, 5, "  ");
        }
        attroff(COLOR_PAIR(COLOR_PAIR_LINE_NUMBERS));
        
        // For continuation lines, just show spaces
        for (int i = 1; i < wrapped_lines && screen_row + i <= visible_lines; i++) {
            mvprintw(screen_row + i, 1, "    ");
            mvprintw(screen_row + i, 5, "  ");
        }
        
        screen_row += wrapped_lines;
        current_line_node = current_line_node->next;
        line_num++;
    }
}

void drawTextContent(int visible_lines, const TextBuffer *buffer) {
    Line *current_line_node = buffer->head;
    int screen_row = 1; // Start from row 1 to leave space for mode indicator
    int max_col = getmaxx(stdscr);
    int text_width = max_col - 8; // Available width for text content

    // Move to the first visible line
    for (int i = 0; i < top_line && current_line_node != NULL; ++i) {
        current_line_node = current_line_node->next;
    }

    while (screen_row <= visible_lines && current_line_node != NULL) {
        // Draw the line with wrapping
        draw_wrapped_line(screen_row, 8, current_line_node->text, text_width, COLOR_PAIR_TEXT);
        
        int wrapped_lines = get_wrapped_line_count(current_line_node->text, text_width);
        screen_row += wrapped_lines;
        current_line_node = current_line_node->next;
    }
}

// Calculate which screen row the cursor should be on
int get_cursor_screen_row(TextBuffer *buffer, int visible_lines) {
    Line *current_line_node = buffer->head;
    int line_count = 0;
    int screen_row = 1; // Start from row 1 to account for mode indicator
    int max_col = getmaxx(stdscr);
    int text_width = max_col - 8;

    // Move to the first visible line
    for (int i = 0; i < top_line && current_line_node != NULL; ++i) {
        current_line_node = current_line_node->next;
        line_count++;
    }

    // Calculate screen position for each line until we reach the cursor line
    while (current_line_node != NULL && current_line_node != buffer->current_line_node) {
        int wrapped_lines = get_wrapped_line_count(current_line_node->text, text_width);
        screen_row += wrapped_lines;
        current_line_node = current_line_node->next;
        line_count++;
    }

    // If we found the cursor line, calculate which wrapped line the cursor is on
    if (current_line_node == buffer->current_line_node && line_wrap_enabled) {
        int cursor_wrapped_line = buffer->current_col_offset / text_width;
        screen_row += cursor_wrapped_line;
    }

    return screen_row;
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
    if (current_mode == MODE_COMMAND && command != NULL) {
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

    // Calculate visible lines (subtract 2: 1 for status bar, 1 for mode indicator)
    int visible_lines = max_row - 2;

    switch (ch) {
    // Vim-style movement keys
    case 'h': // Move left
        if (buffer->current_col_offset > 0) {
            buffer->current_col_offset--;
        }
        break;
    case 'j': // Move down
        if (line->next != NULL) {
            buffer->current_line_node = line->next;
            if (buffer->current_col_offset > buffer->current_line_node->length) {
                buffer->current_col_offset = buffer->current_line_node->length;
            }

            // Check if cursor moved below visible area (accounting for wrapped lines)
            int cursor_screen_row = get_cursor_screen_row(buffer, visible_lines);
            if (cursor_screen_row > visible_lines) {
                top_line++;
            }
        }
        break;
    case 'k': // Move up
    if (line->prev != NULL) {
        buffer->current_line_node = line->prev;
        if (buffer->current_col_offset > buffer->current_line_node->length) {
            buffer->current_col_offset = buffer->current_line_node->length;
        }

        // Corrected logic for scrolling up
        if (get_absolute_line_number(&editor_buffer, buffer->current_line_node) < top_line) {
            top_line--;
        }
    }
    break;
    case 'l': // Move right
        if (buffer->current_col_offset < line->length) {
            buffer->current_col_offset++;
        }
        break;

    // Traditional arrow keys (still supported)
    case KEY_UP:
        handleNormalModeInput('k', buffer);
        break;
    case KEY_DOWN:
        handleNormalModeInput('j', buffer);
        break;
    case KEY_LEFT:
        handleNormalModeInput('h', buffer);
        break;
    case KEY_RIGHT:
        handleNormalModeInput('l', buffer);
        break;

    // Mode switching
    case 'i': // Enter insert mode
        current_mode = MODE_INSERT;
        break;
    case 'a': // Enter insert mode after cursor
        if (buffer->current_col_offset < line->length) {
            buffer->current_col_offset++;
        }
        current_mode = MODE_INSERT;
        break;
    case 'A': // Enter insert mode at end of line
        buffer->current_col_offset = line->length;
        current_mode = MODE_INSERT;
        break;
    case 'o': // Insert new line below and enter insert mode
        {
            Line *new_line = create_new_line("");
            insert_line_after(line, new_line);
            buffer->current_line_node = new_line;
            buffer->current_col_offset = 0;
            current_mode = MODE_INSERT;

            // Check if cursor moved below visible area
            int cursor_screen_row = get_cursor_screen_row(buffer, visible_lines);
            if (cursor_screen_row > visible_lines) {
                top_line++;
            }
        }
        break;
    case 'O': // Insert new line above and enter insert mode
        {
            Line *new_line = create_new_line("");
            if (line->prev != NULL) {
                insert_line_after(line->prev, new_line);
            } else {
                // Insert at beginning
                new_line->next = buffer->head;
                if (buffer->head != NULL) {
                    buffer->head->prev = new_line;
                }
                buffer->head = new_line;
                if (buffer->tail == NULL) {
                    buffer->tail = new_line;
                }
                buffer->num_lines++;
            }
            buffer->current_line_node = new_line;
            buffer->current_col_offset = 0;
            current_mode = MODE_INSERT;

            // Check if cursor moved above visible area
            int cursor_screen_row = get_cursor_screen_row(buffer, visible_lines);
            if (cursor_screen_row < 1) {
                top_line--;
                if (top_line < 0) top_line = 0;
            }
        }
        break;

    case ':': // Enter command mode
        current_mode = MODE_COMMAND;
        break;

    case 27: // Escape key - should stay in normal mode
        current_mode = MODE_NORMAL;
        break;

    // Toggle line wrapping
    case 'w': // Toggle line wrap
        line_wrap_enabled = !line_wrap_enabled;
        break;

    // Normal mode editing commands
    case 'x': // Delete character under cursor
        if (current_col < line->length) {
            memmove(&line->text[current_col], &line->text[current_col + 1],
                   line->length - current_col);
            line->length--;
            line->text = realloc(line->text, line->length + 1);
        }
        break;

    case 'X': // Delete character before cursor
        if (current_col > 0) {
            memmove(&line->text[current_col - 1], &line->text[current_col],
                   line->length - current_col + 1);
            line->length--;
            line->text = realloc(line->text, line->length + 1);
            buffer->current_col_offset--;
        }
        break;

    default:
        // Ignore other keys in normal mode
        break;
    }
}

void handleInsertModeInput(int ch, TextBuffer *buffer) {
    Line *line = buffer->current_line_node;
    size_t current_col = buffer->current_col_offset;
    int max_row, max_col;
    getmaxyx(stdscr, max_row, max_col);

    // Calculate visible lines (subtract 2: 1 for status bar, 1 for mode indicator)
    int visible_lines = max_row - 2;

    switch (ch) {
    case 27: // Escape key - return to normal mode
        current_mode = MODE_NORMAL;
        // Move cursor back one position if possible (vim behavior)
        if (buffer->current_col_offset > 0) {
            buffer->current_col_offset--;
        }
        break;

    case 10: // Enter key
        if (line != NULL) {
            Line *new_line = create_new_line(line->text + current_col);
            // Truncate the current line
            line->text[current_col] = '\0';
            line->length = current_col;

            insert_line_after(line, new_line);
            buffer->current_line_node = new_line;
            buffer->current_col_offset = 0;

            // Check if cursor moved below visible area
            int cursor_screen_row = get_cursor_screen_row(buffer, visible_lines);
            if (cursor_screen_row > visible_lines) {
                top_line++;
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

            // Check scrolling after merge
            int cursor_screen_row = get_cursor_screen_row(buffer, visible_lines);
            if (cursor_screen_row < 1) {
                top_line--;
                if (top_line < 0) top_line = 0;
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

    // Arrow keys work in insert mode
    case KEY_UP:
        if (line->prev != NULL) {
            buffer->current_line_node = line->prev;
            if (buffer->current_col_offset > buffer->current_line_node->length) {
                buffer->current_col_offset = buffer->current_line_node->length;
            }

            int cursor_screen_row = get_cursor_screen_row(buffer, visible_lines);
            if (cursor_screen_row < 1) {
                top_line--;
                if (top_line < 0) top_line = 0;
            }
        }
        break;

    case KEY_DOWN:
        if (line->next != NULL) {
            buffer->current_line_node = line->next;
            if (buffer->current_col_offset > buffer->current_line_node->length) {
                buffer->current_col_offset = buffer->current_line_node->length;
            }

            int cursor_screen_row = get_cursor_screen_row(buffer, visible_lines);
            if (cursor_screen_row > visible_lines) {
                top_line++;
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

void handleCommandModeInput(int ch, char *command, TextBuffer *buffer, const char *filename) {
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
        } else if (strcmp(command, "wrap") == 0) {
            // Toggle line wrapping
            line_wrap_enabled = !line_wrap_enabled;
        } else if (strcmp(command, "nowrap") == 0) {
            // Disable line wrapping
            line_wrap_enabled = 0;
        }

        command[0] = '\0'; // Reset command buffer
        command_index = 0; // Reset command index
        current_mode = MODE_NORMAL;  // Return to normal mode
        break;
    case 27: // Escape key
        command[0] = '\0'; // Reset command buffer
        command_index = 0; // Reset command index
        current_mode = MODE_NORMAL; // Return to normal mode
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
    
    switch (current_mode) {
        case MODE_NORMAL:
            handleNormalModeInput(ch, buffer);
            break;
        case MODE_INSERT:
            handleInsertModeInput(ch, buffer);
            break;
        case MODE_COMMAND:
            handleCommandModeInput(ch, command, buffer, filename);
            break;
    }
}