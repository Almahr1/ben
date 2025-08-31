
#include "data_structures.h"
#include "undo.h"
#include <stdlib.h>
#include <string.h>

UndoStack undo_stack;

void
init_undo_system (void)
{
  undo_stack.head = 0;
  undo_stack.tail = 0;
  undo_stack.current = -1;
  undo_stack.count = 0;

  for (int i = 0; i < MAX_UNDO_OPERATIONS; i++)
    {
      undo_stack.operations[i].is_valid = 0;
      undo_stack.operations[i].target_line = NULL;
    }
}

void
push_undo_operation (UndoType type, Line *target_line, size_t col_pos,
                     const char *data, size_t data_len)
{
  clear_redo_stack ();

  undo_stack.current = (undo_stack.current + 1) % MAX_UNDO_OPERATIONS;

  UndoOperation *op = &undo_stack.operations[undo_stack.current];
  op->type = type;
  op->target_line = target_line;
  op->col_pos = col_pos;
  op->data_len
      = data_len < sizeof (op->data) ? data_len : sizeof (op->data) - 1;
  op->is_valid = 1;

  if (data && op->data_len > 0)
    {
      memcpy (op->data, data, op->data_len);
    }
  op->data[op->data_len] = '\0';

  if (undo_stack.count < MAX_UNDO_OPERATIONS)
    {
      undo_stack.count++;
    }
  else
    {
      undo_stack.tail = (undo_stack.tail + 1) % MAX_UNDO_OPERATIONS;
    }
}

int
is_line_valid_in_buffer (TextBuffer *buffer, Line *target_line)
{
  if (!buffer || !target_line)
    return 0;

  Line *current = buffer->head;
  while (current != NULL)
    {
      if (current == target_line)
        {
          return 1;
        }
      current = current->next;
    }
  return 0;
}

void
invalidate_undo_operations_for_line (Line *deleted_line)
{
  if (!deleted_line)
    return;

  for (int i = 0; i < MAX_UNDO_OPERATIONS; i++)
    {
      if (undo_stack.operations[i].target_line == deleted_line)
        {
          undo_stack.operations[i].is_valid = 0;
        }
    }
}

int
can_undo (void)
{
  if (undo_stack.count <= 0 || undo_stack.current < 0)
    return 0;

  return undo_stack.operations[undo_stack.current].is_valid;
}

int
can_redo (void)
{
  if (undo_stack.current >= undo_stack.count - 1)
    return 0;

  int next_index = undo_stack.current + 1;
  return undo_stack.operations[next_index].is_valid;
}

void
clear_redo_stack (void)
{
  if (undo_stack.current >= 0)
    {
      for (int i = undo_stack.current + 1; i < undo_stack.count; i++)
        {
          undo_stack.operations[i].is_valid = 0;
        }
      undo_stack.count = undo_stack.current + 1;
    }
}

void
validate_cursor_position (TextBuffer *buffer)
{
  if (!buffer)
    return;

  if (!buffer->head)
    {
      Line *initial_line = create_new_line_empty ();
      insert_line_at_end (buffer, initial_line);
      buffer->current_line_node = initial_line;
      buffer->current_col_offset = 0;
      return;
    }

  if (!buffer->current_line_node
      || !is_line_valid_in_buffer (buffer, buffer->current_line_node))
    {
      buffer->current_line_node = buffer->head;
      buffer->current_col_offset = 0;
      return;
    }

  if (buffer->current_line_node)
    {
      size_t line_len = line_get_length (buffer->current_line_node);
      if (buffer->current_col_offset > line_len)
        {
          buffer->current_col_offset = line_len;
        }
    }
}

void
perform_undo (TextBuffer *buffer)
{
  if (!can_undo () || !buffer)
    return;

  UndoOperation *op = &undo_stack.operations[undo_stack.current];

  if (op->type == UNDO_INSERT_LINE && op->target_line == NULL)
    {
      Line *to_remove = buffer->head;
      if (to_remove)
        {
          // Invalidate any operations that reference the line being removed
          invalidate_undo_operations_for_line (to_remove);

          // Update current line pointer if it's pointing to the line being
          // removed
          if (buffer->current_line_node == to_remove)
            {
              buffer->current_line_node = to_remove->next;
              buffer->current_col_offset = 0;
            }

          buffer->head = to_remove->next;
          if (buffer->head)
            {
              buffer->head->prev = NULL;
            }
          else
            {
              buffer->tail = NULL;
            }

          gap_buffer_destroy (to_remove->gb);
          free (to_remove);
          buffer->num_lines--;
        }
      undo_stack.current--;
      validate_cursor_position (buffer);
      return;
    }

  if (op->target_line
      && (!op->is_valid || !is_line_valid_in_buffer (buffer, op->target_line)))
    {
      op->is_valid = 0;
      undo_stack.current--; // Skip this invalid operation
      return;
    }

  Line *target_line = op->target_line;

  switch (op->type)
    {
    case UNDO_INSERT_CHAR:
      if (op->col_pos < line_get_length (target_line))
        {
          line_delete_char_at (target_line, op->col_pos);

          // Update cursor if it's on this line
          if (buffer->current_line_node == target_line
              && buffer->current_col_offset > op->col_pos)
            {
              buffer->current_col_offset--;
            }
        }
      break;

    case UNDO_DELETE_CHAR:
      // Only restore the character if the position is valid and makes sense
      if (op->col_pos <= line_get_length (target_line) && op->data_len > 0)
        {
          line_insert_char_at (target_line, op->col_pos, op->data[0]);

          // Update cursor if it's on this line
          if (buffer->current_line_node == target_line
              && buffer->current_col_offset > op->col_pos)
            {
              buffer->current_col_offset++;
            }
        }
      break;

    case UNDO_INSERT_LINE:
      {
        // Normal case - target_line is not NULL
        Line *to_remove = target_line->next;
        if (to_remove)
          {
            // Invalidate any operations that reference the line being removed
            invalidate_undo_operations_for_line (to_remove);

            // Update current line pointer if it's pointing to the line being
            // removed
            if (buffer->current_line_node == to_remove)
              {
                buffer->current_line_node = target_line;
                buffer->current_col_offset = line_get_length (target_line);
              }

            // Remove the line from the linked list
            if (to_remove->next)
              {
                to_remove->next->prev = target_line;
              }
            target_line->next = to_remove->next;

            if (buffer->tail == to_remove)
              {
                buffer->tail = target_line;
              }

            gap_buffer_destroy (to_remove->gb);
            free (to_remove);
            buffer->num_lines--;
          }
        break;
      }

    case UNDO_DELETE_LINE:
      {
        Line *new_line = create_new_line (op->data);
        if (new_line)
          {
            insert_line_after_buffer (buffer, target_line, new_line);
          }
        break;
      }

    case UNDO_SPLIT_LINE:
      {
        Line *second_line = target_line->next;
        if (second_line)
          {
            char *second_content = line_to_string (second_line);
            if (second_content)
              {
                // Invalidate operations that reference the second line
                invalidate_undo_operations_for_line (second_line);

                // Update cursor if it's on the second line
                if (buffer->current_line_node == second_line)
                  {
                    buffer->current_line_node = target_line;
                    buffer->current_col_offset
                        = op->col_pos + buffer->current_col_offset;
                  }

                // Merge the lines back together
                line_insert_string_at (target_line, op->col_pos,
                                       second_content);
                free (second_content);

                // Remove the second line
                target_line->next = second_line->next;
                if (second_line->next)
                  {
                    second_line->next->prev = target_line;
                  }
                else
                  {
                    buffer->tail = target_line;
                  }

                gap_buffer_destroy (second_line->gb);
                free (second_line);
                buffer->num_lines--;
              }
          }
        break;
      }

    case UNDO_MERGE_LINES:
      {
        // Split the line back at the merge point
        size_t split_pos = op->col_pos;
        char *line_text = line_to_string (target_line);

        if (line_text && strlen (line_text) >= split_pos)
          {
            Line *new_line = create_new_line (line_text + split_pos);

            if (new_line)
              {
                // Truncate the original line
                gap_buffer_move_cursor_to (target_line->gb, split_pos);
                size_t chars_to_delete
                    = line_get_length (target_line) - split_pos;
                for (size_t i = 0; i < chars_to_delete; i++)
                  {
                    gap_buffer_delete_char (target_line->gb);
                  }

                // Insert the new line
                insert_line_after_buffer (buffer, target_line, new_line);

                // Update cursor if it was beyond the split point
                if (buffer->current_line_node == target_line
                    && buffer->current_col_offset > split_pos)
                  {
                    buffer->current_line_node = new_line;
                    buffer->current_col_offset -= split_pos;
                  }
              }
            free (line_text);
          }
        break;
      }
    }

  undo_stack.current--;

  // Always validate cursor position after undo
  validate_cursor_position (buffer);
}

void
perform_redo (TextBuffer *buffer)
{
  if (!can_redo () || !buffer)
    return;

  undo_stack.current++;
  UndoOperation *op = &undo_stack.operations[undo_stack.current];

  // Validate the target line still exists in the buffer
  if (!op->is_valid || !is_line_valid_in_buffer (buffer, op->target_line))
    {
      op->is_valid = 0;
      undo_stack.current--;
      return;
    }

  Line *target_line = op->target_line;

  switch (op->type)
    {
    case UNDO_INSERT_CHAR:
      if (op->col_pos <= line_get_length (target_line))
        {
          line_insert_char_at (target_line, op->col_pos, op->data[0]);

          // Update cursor if it's on this line
          if (buffer->current_line_node == target_line
              && buffer->current_col_offset > op->col_pos)
            {
              buffer->current_col_offset++;
            }
        }
      break;

    case UNDO_DELETE_CHAR:
      if (op->col_pos < line_get_length (target_line))
        {
          line_delete_char_at (target_line, op->col_pos);

          // Update cursor if it's on this line
          if (buffer->current_line_node == target_line
              && buffer->current_col_offset > op->col_pos)
            {
              buffer->current_col_offset--;
            }
        }
      break;

    case UNDO_INSERT_LINE:
      {
        Line *new_line = create_new_line (op->data);
        if (new_line)
          {
            insert_line_after_buffer (buffer, target_line, new_line);
          }
        break;
      }

    case UNDO_DELETE_LINE:
      {
        Line *to_remove = target_line->next;
        if (to_remove)
          {
            // Invalidate any operations that reference the line being removed
            invalidate_undo_operations_for_line (to_remove);

            // Update current line pointer if it's pointing to the line being
            // removed
            if (buffer->current_line_node == to_remove)
              {
                buffer->current_line_node = target_line;
                buffer->current_col_offset = line_get_length (target_line);
              }

            // Remove the line
            if (to_remove->next)
              {
                to_remove->next->prev = target_line;
              }
            target_line->next = to_remove->next;

            if (buffer->tail == to_remove)
              {
                buffer->tail = target_line;
              }

            gap_buffer_destroy (to_remove->gb);
            free (to_remove);
            buffer->num_lines--;
          }
        break;
      }

    case UNDO_SPLIT_LINE:
      {
        // Split the line at the specified position
        size_t split_pos = op->col_pos;
        char *line_text = line_to_string (target_line);

        if (line_text && strlen (line_text) >= split_pos)
          {
            Line *new_line = create_new_line (line_text + split_pos);

            if (new_line)
              {
                // Truncate the original line
                gap_buffer_move_cursor_to (target_line->gb, split_pos);
                size_t chars_to_delete
                    = line_get_length (target_line) - split_pos;
                for (size_t i = 0; i < chars_to_delete; i++)
                  {
                    gap_buffer_delete_char (target_line->gb);
                  }

                // Insert the new line
                insert_line_after_buffer (buffer, target_line, new_line);

                // Update cursor if it was beyond the split point
                if (buffer->current_line_node == target_line
                    && buffer->current_col_offset > split_pos)
                  {
                    buffer->current_line_node = new_line;
                    buffer->current_col_offset -= split_pos;
                  }
              }
            free (line_text);
          }
        break;
      }

    case UNDO_MERGE_LINES:
      {
        Line *second_line = target_line->next;
        if (second_line)
          {
            char *second_content = line_to_string (second_line);
            if (second_content)
              {
                // Invalidate operations that reference the second line
                invalidate_undo_operations_for_line (second_line);

                // Update cursor if it's on the second line
                if (buffer->current_line_node == second_line)
                  {
                    buffer->current_line_node = target_line;
                    buffer->current_col_offset = line_get_length (target_line)
                                                 + buffer->current_col_offset;
                  }

                // Merge the lines
                line_insert_string_at (target_line,
                                       line_get_length (target_line),
                                       second_content);
                free (second_content);

                // Remove the second line
                target_line->next = second_line->next;
                if (second_line->next)
                  {
                    second_line->next->prev = target_line;
                  }
                else
                  {
                    buffer->tail = target_line;
                  }

                gap_buffer_destroy (second_line->gb);
                free (second_line);
                buffer->num_lines--;
              }
          }
        break;
      }
    }

  // Always validate cursor position after redo
  validate_cursor_position (buffer);
}

// Legacy functions for compatibility (now implemented using line pointers)
size_t
get_line_number (TextBuffer *buffer, Line *target)
{
  if (!buffer || !target)
    return 0;

  Line *current = buffer->head;
  size_t line_num = 0;

  while (current != NULL && current != target)
    {
      current = current->next;
      line_num++;
    }

  // If target not found, return a safe value
  if (current != target)
    {
      return 0;
    }

  return line_num;
}

Line *
get_line_by_number (TextBuffer *buffer, size_t line_num)
{
  if (!buffer || !buffer->head)
    return NULL;

  Line *current = buffer->head;
  size_t count = 0;

  while (current != NULL && count < line_num)
    {
      current = current->next;
      count++;
    }

  return current;
}
