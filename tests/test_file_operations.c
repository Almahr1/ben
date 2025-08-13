#include "test_framework.h"
#include "../src/file_operations.c"
#include "../src/data_structures.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// A global filename for testing
const char* TEST_FILENAME = "test_file.txt";

// Declare the global editor_buffer to be used by the test functions
TextBuffer editor_buffer;

void test_save_and_load_file(void) {
    // Test saving and loading a file with multiple lines
    TEST_CASE_START("saveToFile and loadFromFile - multiple lines");
    
    // Initialize the global editor_buffer
    init_editor_buffer();
    Line *line1 = create_new_line("Hello world!");
    editor_buffer.current_line_node = line1;
    insert_line_at_end(&editor_buffer, line1);
    Line *line2 = create_new_line("This is a test file.");
    insert_line_at_end(&editor_buffer, line2);
    Line *line3 = create_new_line("");
    insert_line_at_end(&editor_buffer, line3);
    Line *line4 = create_new_line("The end.");
    insert_line_at_end(&editor_buffer, line4);

    // Save the buffer to a file, passing the buffer as the second argument
    saveToFile(TEST_FILENAME, &editor_buffer);

    // Free the current buffer and re-initialize it for the load test
    free_editor_buffer(&editor_buffer);
    init_editor_buffer();

    // Load the file into the global buffer, passing the buffer as the second argument
    loadFromFile(TEST_FILENAME, &editor_buffer);
    
    // Assert that the content and number of lines are correct
    ASSERT_EQ(4, editor_buffer.num_lines, "Loaded buffer should have 4 lines");
    
    Line* current_line = editor_buffer.head;
    char* content1 = line_to_string(current_line);
    ASSERT_STR_EQ("Hello world!", content1, "First line content should match");
    free(content1);
    current_line = current_line->next;

    char* content2 = line_to_string(current_line);
    ASSERT_STR_EQ("This is a test file.", content2, "Second line content should match");
    free(content2);
    current_line = current_line->next;
    
    char* content3 = line_to_string(current_line);
    ASSERT_STR_EQ("", content3, "Third line should be empty");
    free(content3);
    current_line = current_line->next;

    char* content4 = line_to_string(current_line);
    ASSERT_STR_EQ("The end.", content4, "Fourth line content should match");
    free(content4);

    // Clean up
    free_editor_buffer(&editor_buffer);
    remove(TEST_FILENAME); // Delete the test file
    TEST_CASE_END();
}

void test_load_empty_file(void) {
    // Test loading a non-existent or empty file
    TEST_CASE_START("loadFromFile - empty file");
    
    // Initialize the global editor_buffer
    init_editor_buffer();
    
    // Create an empty file
    FILE *fp = fopen(TEST_FILENAME, "w");
    if (fp) fclose(fp);

    // Load the file, passing the buffer as the second argument
    loadFromFile(TEST_FILENAME, &editor_buffer);

    // Assert that a single, empty line is created
    ASSERT_EQ(1, editor_buffer.num_lines, "Loaded buffer should have 1 line for an empty file");
    ASSERT_NOT_NULL(editor_buffer.head, "Head should not be NULL");
    ASSERT_NOT_NULL(editor_buffer.current_line_node, "Current line node should not be NULL");
    
    char* content = line_to_string(editor_buffer.head);
    ASSERT_STR_EQ("", content, "The line should be empty");
    free(content); // Free the string from line_to_string

    // Clean up
    free_editor_buffer(&editor_buffer);
    remove(TEST_FILENAME);
    TEST_CASE_END();
}

void run_file_operations_tests(void) {
    TEST_SUITE_START("File Operations Tests");
    init_editor_buffer();
    
    test_save_and_load_file();
    test_load_empty_file();
    
    TEST_SUITE_END("File Operations Tests");
}