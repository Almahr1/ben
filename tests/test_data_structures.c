#include "test_framework.h"
#include "../src/data_structures.h"
#include "../src/gap_buffer.h"
#include <string.h>

void test_line_creation(void) {
    // Test creating line with content
    Line *line = create_new_line("Hello World");
    ASSERT_NOT_NULL(line, "Line should be created successfully");
    ASSERT_NOT_NULL(line->gb, "Line should have a gap buffer");
    ASSERT_EQ(11, line_get_length(line), "Line should have correct length");
    
    char *content = line_to_string(line);
    ASSERT_STR_EQ("Hello World", content, "Line should contain correct content");
    free(content);
    
    gap_buffer_destroy(line->gb);
    free(line);
    
    // Test creating empty line
    Line *empty_line = create_new_line_empty();
    ASSERT_NOT_NULL(empty_line, "Empty line should be created successfully");
    ASSERT_NOT_NULL(empty_line->gb, "Empty line should have a gap buffer");
    ASSERT_EQ(0, line_get_length(empty_line), "Empty line should have length 0");
    
    gap_buffer_destroy(empty_line->gb);
    free(empty_line);
}

void test_line_operations(void) {
    Line *line = create_new_line("Hello");
    
    // Test getting character at position
    ASSERT_EQ('H', line_get_char_at(line, 0), "First character should be 'H'");
    ASSERT_EQ('e', line_get_char_at(line, 1), "Second character should be 'e'");
    ASSERT_EQ('\0', line_get_char_at(line, 10), "Character beyond line should be null");
    
    // Test inserting character
    line_insert_char_at(line, 5, '!');
    ASSERT_EQ(6, line_get_length(line), "Line length should increase after insertion");
    
    char *content = line_to_string(line);
    ASSERT_STR_EQ("Hello!", content, "Line should contain inserted character");
    free(content);
    
    // Test inserting string
    line_insert_string_at(line, 5, " World");
    ASSERT_EQ(12, line_get_length(line), "Line length should increase after string insertion");
    
    content = line_to_string(line);
    ASSERT_STR_EQ("Hello World!", content, "Line should contain inserted string");
    free(content);
    
    // Test deleting character
    line_delete_char_at(line, 5);
    ASSERT_EQ(11, line_get_length(line), "Line length should decrease after deletion");
    
    content = line_to_string(line);
    ASSERT_STR_EQ("HelloWorld!", content, "Character should be deleted");
    free(content);
    
    // Test deleting character before
    line_delete_char_before(line, 5);
    ASSERT_EQ(10, line_get_length(line), "Line length should decrease after backspace deletion");
    
    content = line_to_string(line);
    ASSERT_STR_EQ("HellWorld!", content, "Character should be deleted with backspace");
    free(content);
    
    gap_buffer_destroy(line->gb);
    free(line);
}

void test_text_buffer_initialization(void) {
    TextBuffer buffer;
    
    // Initialize buffer
    buffer.head = NULL;
    buffer.tail = NULL;
    buffer.num_lines = 0;
    buffer.current_line_node = NULL;
    buffer.current_col_offset = 0;
    
    ASSERT_NULL(buffer.head, "Buffer head should be NULL after initialization");
    ASSERT_NULL(buffer.tail, "Buffer tail should be NULL after initialization");
    ASSERT_EQ(0, buffer.num_lines, "Buffer should have 0 lines after initialization");
    ASSERT_NULL(buffer.current_line_node, "Current line should be NULL after initialization");
    ASSERT_EQ(0, buffer.current_col_offset, "Column offset should be 0 after initialization");
}

void test_line_insertion(void) {
    TextBuffer buffer;
    buffer.head = NULL;
    buffer.tail = NULL;
    buffer.num_lines = 0;
    buffer.current_line_node = NULL;
    buffer.current_col_offset = 0;
    
    // Test inserting first line
    Line *line1 = create_new_line("First line");
    insert_line_at_end(&buffer, line1);
    
    ASSERT_EQ(line1, buffer.head, "First line should be buffer head");
    ASSERT_EQ(line1, buffer.tail, "First line should be buffer tail");
    ASSERT_EQ(1, buffer.num_lines, "Buffer should have 1 line");
    ASSERT_NULL(line1->prev, "First line should have no previous line");
    ASSERT_NULL(line1->next, "First line should have no next line");
    
    // Test inserting second line
    Line *line2 = create_new_line("Second line");
    insert_line_at_end(&buffer, line2);
    
    ASSERT_EQ(line1, buffer.head, "First line should still be buffer head");
    ASSERT_EQ(line2, buffer.tail, "Second line should be buffer tail");
    ASSERT_EQ(2, buffer.num_lines, "Buffer should have 2 lines");
    ASSERT_EQ(line2, line1->next, "First line should point to second line");
    ASSERT_EQ(line1, line2->prev, "Second line should point to first line");
    ASSERT_NULL(line2->next, "Second line should have no next line");
    
    // Test inserting line after first line
    Line *line3 = create_new_line("Third line");
    insert_line_after(line1, line3);
    
    ASSERT_EQ(3, buffer.num_lines, "Buffer should have 3 lines");
    ASSERT_EQ(line3, line1->next, "Line 3 should come after line 1");
    ASSERT_EQ(line2, line3->next, "Line 2 should come after line 3");
    ASSERT_EQ(line3, line2->prev, "Line 2 should point back to line 3");
    ASSERT_EQ(line1, line3->prev, "Line 3 should point back to line 1");
    
    // Clean up
    free_editor_buffer(&buffer);
}

void test_buffer_traversal(void) {
    TextBuffer buffer;
    buffer.head = NULL;
    buffer.tail = NULL;
    buffer.num_lines = 0;
    buffer.current_line_node = NULL;
    buffer.current_col_offset = 0;
    
    // Create a buffer with multiple lines
    Line *lines[5];
    for (int i = 0; i < 5; i++) {
        char content[20];
        snprintf(content, sizeof(content), "Line %d", i + 1);
        lines[i] = create_new_line(content);
        insert_line_at_end(&buffer, lines[i]);
    }
    
    // Test forward traversal
    Line *current = buffer.head;
    int count = 0;
    while (current != NULL) {
        char expected[20];
        snprintf(expected, sizeof(expected), "Line %d", count + 1);
        char *actual = line_to_string(current);
        
        ASSERT_STR_EQ(expected, actual, "Forward traversal should visit lines in order");
        free(actual);
        
        current = current->next;
        count++;
    }
    ASSERT_EQ(5, count, "Forward traversal should visit all 5 lines");
    
    // Test backward traversal
    current = buffer.tail;
    count = 4;
    while (current != NULL) {
        char expected[20];
        snprintf(expected, sizeof(expected), "Line %d", count + 1);
        char *actual = line_to_string(current);
        
        ASSERT_STR_EQ(expected, actual, "Backward traversal should visit lines in reverse order");
        free(actual);
        
        current = current->prev;
        count--;
    }
    ASSERT_EQ(-1, count, "Backward traversal should visit all 5 lines");
    
    // Clean up
    free_editor_buffer(&buffer);
}

void test_buffer_cleanup(void) {
    TextBuffer buffer;
    buffer.head = NULL;
    buffer.tail = NULL;
    buffer.num_lines = 0;
    buffer.current_line_node = NULL;
    buffer.current_col_offset = 0;
    
    // Create a buffer with lines
    for (int i = 0; i < 3; i++) {
        char content[20];
        snprintf(content, sizeof(content), "Line %d", i + 1);
        Line *line = create_new_line(content);
        insert_line_at_end(&buffer, line);
    }
    
    ASSERT_EQ(3, buffer.num_lines, "Buffer should have 3 lines before cleanup");
    
    // Free the buffer
    free_editor_buffer(&buffer);
    
    ASSERT_NULL(buffer.head, "Buffer head should be NULL after cleanup");
    ASSERT_NULL(buffer.tail, "Buffer tail should be NULL after cleanup");
    ASSERT_EQ(0, buffer.num_lines, "Buffer should have 0 lines after cleanup");
    ASSERT_NULL(buffer.current_line_node, "Current line should be NULL after cleanup");
    ASSERT_EQ(0, buffer.current_col_offset, "Column offset should be 0 after cleanup");
}

void test_line_edge_cases(void) {
    // Test operations on NULL line
    ASSERT_EQ(0, line_get_length(NULL), "Getting length of NULL line should return 0");
    ASSERT_EQ('\0', line_get_char_at(NULL, 0), "Getting char from NULL line should return null char");
    ASSERT_NULL(line_to_string(NULL), "Converting NULL line to string should return NULL");
    
    // Test operations at boundary positions
    Line *line = create_new_line("test");
    
    // Test deleting at position 0 with backspace (should do nothing)
    line_delete_char_before(line, 0);
    ASSERT_EQ(4, line_get_length(line), "Line length should remain unchanged");
    
    char *content = line_to_string(line);
    ASSERT_STR_EQ("test", content, "Line content should remain unchanged");
    free(content);
    
    gap_buffer_destroy(line->gb);
    free(line);
}

void run_data_structures_tests(void) {
    TEST_SUITE_START("Data Structures Tests");
    
    test_line_creation();
    test_line_operations();
    test_text_buffer_initialization();
    test_line_insertion();
    test_buffer_traversal();
    test_buffer_cleanup();
    test_line_edge_cases();
    
    TEST_SUITE_END("Data Structures Tests");
}