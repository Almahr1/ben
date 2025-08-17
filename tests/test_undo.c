#include "../src/undo.h"
#include "test_framework.h"
#include "../src/data_structures.h"

void test_undo_insert_char(void){
    TextBuffer buffer;
    init_editor_buffer(&buffer);
    init_undo_system();
    
    // Create a line with content
    Line *line = create_new_line("hello");
    insert_line_at_end(&buffer, line);
    buffer.current_line_node = line;

    // Should be doable operation
    push_undo_operation(UNDO_INSERT_CHAR, line, 5, "!", 1);
    line_insert_char_at(line, 5, '!');

    ASSERT_TRUE(can_undo(), "Should be Able to Undo");

    char* content = line_to_string(line); // This should be "hello"     
    ASSERT_STR_EQ("hello", content, "Undo Should Restore Original Content");

    free(content);
    
    free_editor_buffer(&buffer);
}


void test_undo_delete_char(void) {
    // Test UNDO_DELETE_CHAR operations
}

void test_undo_insert_line(void) {
    // Test UNDO_INSERT_LINE operations
}

void test_undo_delete_line(void) {
    // Test UNDO_DELETE_LINE operations
}

void test_undo_split_line(void) {
    // Test UNDO_SPLIT_LINE operations
}

void test_undo_merge_lines(void) {
    // Test UNDO_MERGE_LINES operations
}

void test_undo_complex_scenarios(void) {
    // Test mixed operations, sequences, edge cases
}

void test_undo_edge_cases(void) {
    // Test invalid references, buffer boundaries, etc.
}

void run_undo_tests(void) {
    TEST_SUITE_START("Undo System Tests");
    
    test_undo_insert_char();
    test_undo_delete_char();
    test_undo_insert_line();
    test_undo_delete_line();
    test_undo_split_line();
    test_undo_merge_lines();
    test_undo_complex_scenarios();
    test_undo_edge_cases();
    
    TEST_SUITE_END("Undo System Tests");
}