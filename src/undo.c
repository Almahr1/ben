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
    Line *current = buffer->head;
    size_t line_num = 0;
    
    while (current != NULL && current != target) {
        current = current->next;
        line_num++;
    }
    
    return line_num;
}

Line* get_line_by_number(TextBuffer *buffer, size_t line_num) {
    Line *current = buffer->head;
    size_t count = 0;
    
    while (current != NULL && count < line_num) {
        current = current->next;
        count++;
    }
    
    return current;
}

void perform_undo(TextBuffer *buffer) {
    if (!can_undo()) return;
    
    UndoOperation *op = &undo_stack.operations[undo_stack.current];
    Line *target_line = get_line_by_number(buffer, op->line_num);
    
    if (!target_line) return;
    
    switch (op->type) {
        case UNDO_INSERT_CHAR:
            line_delete_char_at(target_line, op->col_pos);
            break;
            
        case UNDO_DELETE_CHAR:
            line_insert_char_at(target_line, op->col_pos, op->data[0]);
            break;
            
        case UNDO_INSERT_LINE: {
            Line *to_remove = target_line->next;
            if (to_remove) {
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
            insert_line_after_buffer(buffer, target_line, new_line);
            break;
        }
        
        case UNDO_SPLIT_LINE: {
            Line *second_line = target_line->next;
            if (second_line) {
                char *second_content = line_to_string(second_line);
                if (second_content) {
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
                
                gap_buffer_move_cursor_to(target_line->gb, split_pos);
                size_t chars_to_delete = line_get_length(target_line) - split_pos;
                for (size_t i = 0; i < chars_to_delete; i++) {
                    gap_buffer_delete_char(target_line->gb);
                }
                
                insert_line_after_buffer(buffer, target_line, new_line);
                free(line_text);
            }
            break;
        }
    }
    
    undo_stack.current--;
}

void perform_redo(TextBuffer *buffer) {
    if (!can_redo()) return;
    
    undo_stack.current++;
    UndoOperation *op = &undo_stack.operations[undo_stack.current];
    Line *target_line = get_line_by_number(buffer, op->line_num);
    
    if (!target_line) return;
    
    switch (op->type) {
        case UNDO_INSERT_CHAR:
            line_insert_char_at(target_line, op->col_pos, op->data[0]);
            break;
            
        case UNDO_DELETE_CHAR:
            line_delete_char_at(target_line, op->col_pos);
            break;
            
        case UNDO_INSERT_LINE: {
            Line *new_line = create_new_line(op->data);
            insert_line_after_buffer(buffer, target_line, new_line);
            break;
        }
        
        case UNDO_DELETE_LINE: {
            Line *to_remove = target_line->next;
            if (to_remove) {
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
                
                gap_buffer_move_cursor_to(target_line->gb, split_pos);
                size_t chars_to_delete = line_get_length(target_line) - split_pos;
                for (size_t i = 0; i < chars_to_delete; i++) {
                    gap_buffer_delete_char(target_line->gb);
                }
                
                insert_line_after_buffer(buffer, target_line, new_line);
                free(line_text);
            }
            break;
        }
        
        case UNDO_MERGE_LINES: {
            Line *second_line = target_line->next;
            if (second_line) {
                char *second_content = line_to_string(second_line);
                if (second_content) {
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
}