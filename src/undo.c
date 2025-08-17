// Fixed undo.c - Key improvements to prevent segfaults

#include "undo.h"
#include "data_structures.h"
#include "text_editor_functions.h"
#include <string.h>
#include <stdlib.h>

UndoStack undo_stack;

void init_undo_system(void) {
    undo_stack.head = 0;
    undo_stack.tail = 0;
    undo_stack.current = -1;
    undo_stack.count = 0;
}

void push_undo_operation(UndoType type, size_t line_num, size_t col_pos, const char *data, size_t data_len) {
    clear_redo_stack();
    
    undo_stack.current = (undo_stack.current + 1) % MAX_UNDO_OPERATIONS;
    
    UndoOperation *op = &undo_stack.operations[undo_stack.current];
    op->type = type;
    op->line_num = line_num;
    op->col_pos = col_pos;
    op->data_len = data_len < sizeof(op->data) ? data_len : sizeof(op->data) - 1;
    
    if (data && op->data_len > 0) {
        memcpy(op->data, data, op->data_len);
    }
    op->data[op->data_len] = '\0';
    
    if (undo_stack.count < MAX_UNDO_OPERATIONS) {
        undo_stack.count++;
    } else {
        undo_stack.tail = (undo_stack.tail + 1) % MAX_UNDO_OPERATIONS;
    }
}

int can_undo(void) {
    return undo_stack.count > 0 && undo_stack.current >= 0;
}

int can_redo(void) {
    return undo_stack.current < undo_stack.count - 1;
}

void clear_redo_stack(void) {
    if (undo_stack.current >= 0) {
        undo_stack.count = undo_stack.current + 1;
    }
}

size_t get_line_number(TextBuffer *buffer, Line *target) {
    if (!buffer || !target) return 0;
    
    Line *current = buffer->head;
    size_t line_num = 0;
    
    while (current != NULL && current != target) {
        current = current->next;
        line_num++;
    }
    
    // If target not found, return a safe value
    if (current != target) {
        return 0;
    }
    
    return line_num;
}

Line* get_line_by_number(TextBuffer *buffer, size_t line_num) {
    if (!buffer || !buffer->head) return NULL;
    
    Line *current = buffer->head;
    size_t count = 0;
    
    while (current != NULL && count < line_num) {
        current = current->next;
        count++;
    }
    
    return current;
}

// Helper function to safely validate and fix cursor position
void validate_cursor_position(TextBuffer *buffer) {
    if (!buffer) return;
    
    // Ensure we have at least one line
    if (!buffer->head) {
        Line *initial_line = create_new_line_empty();
        insert_line_at_end(buffer, initial_line);
        buffer->current_line_node = initial_line;
        buffer->current_col_offset = 0;
        return;
    }
    
    // Check if current line node is valid
    if (!buffer->current_line_node) {
        buffer->current_line_node = buffer->head;
        buffer->current_col_offset = 0;
        return;
    }
    
    // Verify the current line node is actually in the buffer
    Line *current = buffer->head;
    int found = 0;
    while (current) {
        if (current == buffer->current_line_node) {
            found = 1;
            break;
        }
        current = current->next;
    }
    
    if (!found) {
        // Current line node is dangling, reset to head
        buffer->current_line_node = buffer->head;
        buffer->current_col_offset = 0;
    }
    
    // Ensure column offset is within bounds
    if (buffer->current_line_node) {
        size_t line_len = line_get_length(buffer->current_line_node);
        if (buffer->current_col_offset > line_len) {
            buffer->current_col_offset = line_len;
        }
    }
}

void perform_undo(TextBuffer *buffer) {
    if (!can_undo() || !buffer) return;
    
    UndoOperation *op = &undo_stack.operations[undo_stack.current];
    
    // Validate line number before getting the line
    if (op->line_num >= buffer->num_lines) {
        undo_stack.current--; // Skip this invalid operation
        return;
    }
    
    Line *target_line = get_line_by_number(buffer, op->line_num);
    if (!target_line) {
        undo_stack.current--; // Skip this invalid operation
        return;
    }
    
    switch (op->type) {
        case UNDO_INSERT_CHAR:
            if (op->col_pos < line_get_length(target_line)) {
                line_delete_char_at(target_line, op->col_pos);
            }
            break;
            
        case UNDO_DELETE_CHAR:
            if (op->col_pos <= line_get_length(target_line)) {
                line_insert_char_at(target_line, op->col_pos, op->data[0]);
            }
            break;
            
        case UNDO_INSERT_LINE: {
            Line *to_remove = target_line->next;
            if (to_remove) {
                // Update current line pointer if it's pointing to the line being removed
                if (buffer->current_line_node == to_remove) {
                    buffer->current_line_node = target_line;
                    buffer->current_col_offset = line_get_length(target_line);
                }
                
                if (to_remove->next) {
                    to_remove->next->prev = target_line;
                }
                target_line->next = to_remove->next;
                
                if (buffer->tail == to_remove) {
                    buffer->tail = target_line;
                }
                
                gap_buffer_destroy(to_remove->gb);
                free(to_remove);
                buffer->num_lines--;
            }
            break;
        }
        
        case UNDO_DELETE_LINE: {
            Line *new_line = create_new_line(op->data);
            if (new_line) {
                insert_line_after_buffer(buffer, target_line, new_line);
            }
            break;
        }
        
        case UNDO_SPLIT_LINE: {
            Line *second_line = target_line->next;
            if (second_line) {
                char *second_content = line_to_string(second_line);
                if (second_content) {
                    // Update cursor if it's on the second line
                    if (buffer->current_line_node == second_line) {
                        buffer->current_line_node = target_line;
                        buffer->current_col_offset = line_get_length(target_line) + buffer->current_col_offset;
                    }
                    
                    line_insert_string_at(target_line, line_get_length(target_line), second_content);
                    free(second_content);
                    
                    target_line->next = second_line->next;
                    if (second_line->next) {
                        second_line->next->prev = target_line;
                    } else {
                        buffer->tail = target_line;
                    }
                    
                    gap_buffer_destroy(second_line->gb);
                    free(second_line);
                    buffer->num_lines--;
                }
            }
            break;
        }
        
        case UNDO_MERGE_LINES: {
            size_t split_pos = op->col_pos;
            char *line_text = line_to_string(target_line);
            
            if (line_text && strlen(line_text) >= split_pos) {
                Line *new_line = create_new_line(line_text + split_pos);
                
                if (new_line) {
                    gap_buffer_move_cursor_to(target_line->gb, split_pos);
                    size_t chars_to_delete = line_get_length(target_line) - split_pos;
                    for (size_t i = 0; i < chars_to_delete; i++) {
                        gap_buffer_delete_char(target_line->gb);
                    }
                    
                    insert_line_after_buffer(buffer, target_line, new_line);
                    
                    // Update cursor if it was beyond the split point
                    if (buffer->current_line_node == target_line && 
                        buffer->current_col_offset > split_pos) {
                        buffer->current_line_node = new_line;
                        buffer->current_col_offset -= split_pos;
                    }
                }
                free(line_text);
            }
            break;
        }
    }
    
    undo_stack.current--;
    
    // Always validate cursor position after undo
    validate_cursor_position(buffer);
}

void perform_redo(TextBuffer *buffer) {
    if (!can_redo() || !buffer) return;
    
    undo_stack.current++;
    UndoOperation *op = &undo_stack.operations[undo_stack.current];
    
    // Validate line number before getting the line
    if (op->line_num >= buffer->num_lines) {
        undo_stack.current--; // Revert the increment
        return;
    }
    
    Line *target_line = get_line_by_number(buffer, op->line_num);
    if (!target_line) {
        undo_stack.current--; // Revert the increment
        return;
    }
    
    switch (op->type) {
        case UNDO_INSERT_CHAR:
            if (op->col_pos <= line_get_length(target_line)) {
                line_insert_char_at(target_line, op->col_pos, op->data[0]);
            }
            break;
            
        case UNDO_DELETE_CHAR:
            if (op->col_pos < line_get_length(target_line)) {
                line_delete_char_at(target_line, op->col_pos);
            }
            break;
            
        case UNDO_INSERT_LINE: {
            Line *new_line = create_new_line(op->data);
            if (new_line) {
                insert_line_after_buffer(buffer, target_line, new_line);
            }
            break;
        }
        
        case UNDO_DELETE_LINE: {
            Line *to_remove = target_line->next;
            if (to_remove) {
                // Update current line pointer if it's pointing to the line being removed
                if (buffer->current_line_node == to_remove) {
                    buffer->current_line_node = target_line;
                    buffer->current_col_offset = line_get_length(target_line);
                }
                
                if (to_remove->next) {
                    to_remove->next->prev = target_line;
                }
                target_line->next = to_remove->next;
                
                if (buffer->tail == to_remove) {
                    buffer->tail = target_line;
                }
                
                gap_buffer_destroy(to_remove->gb);
                free(to_remove);
                buffer->num_lines--;
            }
            break;
        }
        
        case UNDO_SPLIT_LINE: {
            size_t split_pos = op->col_pos;
            char *line_text = line_to_string(target_line);
            
            if (line_text && strlen(line_text) >= split_pos) {
                Line *new_line = create_new_line(line_text + split_pos);
                
                if (new_line) {
                    gap_buffer_move_cursor_to(target_line->gb, split_pos);
                    size_t chars_to_delete = line_get_length(target_line) - split_pos;
                    for (size_t i = 0; i < chars_to_delete; i++) {
                        gap_buffer_delete_char(target_line->gb);
                    }
                    
                    insert_line_after_buffer(buffer, target_line, new_line);
                    
                    // Update cursor if it was beyond the split point
                    if (buffer->current_line_node == target_line && 
                        buffer->current_col_offset > split_pos) {
                        buffer->current_line_node = new_line;
                        buffer->current_col_offset -= split_pos;
                    }
                }
                free(line_text);
            }
            break;
        }
        
        case UNDO_MERGE_LINES: {
            Line *second_line = target_line->next;
            if (second_line) {
                char *second_content = line_to_string(second_line);
                if (second_content) {
                    // Update cursor if it's on the second line
                    if (buffer->current_line_node == second_line) {
                        buffer->current_line_node = target_line;
                        buffer->current_col_offset = line_get_length(target_line) + buffer->current_col_offset;
                    }
                    
                    line_insert_string_at(target_line, line_get_length(target_line), second_content);
                    free(second_content);
                    
                    target_line->next = second_line->next;
                    if (second_line->next) {
                        second_line->next->prev = target_line;
                    } else {
                        buffer->tail = target_line;
                    }
                    
                    gap_buffer_destroy(second_line->gb);
                    free(second_line);
                    buffer->num_lines--;
                }
            }
            break;
        }
    }
    
    // Always validate cursor position after redo
    validate_cursor_position(buffer);
}