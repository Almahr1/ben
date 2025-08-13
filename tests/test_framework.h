#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ANSI color codes for better test output
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RESET   "\033[0m"

// Global test counters
extern int tests_run;
extern int tests_passed;
extern int tests_failed;

// Test macros
#define ASSERT_TRUE(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf(COLOR_GREEN "✓ " COLOR_RESET "%s\n", message); \
        } else { \
            tests_failed++; \
            printf(COLOR_RED "✗ " COLOR_RESET "%s\n", message); \
            printf(COLOR_RED "  Failed at %s:%d\n" COLOR_RESET, __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_FALSE(condition, message) \
    ASSERT_TRUE(!(condition), message)

#define ASSERT_EQ(expected, actual, message) \
    do { \
        tests_run++; \
        if ((expected) == (actual)) { \
            tests_passed++; \
            printf(COLOR_GREEN "✓ " COLOR_RESET "%s\n", message); \
        } else { \
            tests_failed++; \
            printf(COLOR_RED "✗ " COLOR_RESET "%s\n", message); \
            printf(COLOR_RED "  Expected: %d, Got: %d at %s:%d\n" COLOR_RESET, \
                   (int)(expected), (int)(actual), __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_STR_EQ(expected, actual, message) \
    do { \
        tests_run++; \
        if (strcmp((expected), (actual)) == 0) { \
            tests_passed++; \
            printf(COLOR_GREEN "✓ " COLOR_RESET "%s\n", message); \
        } else { \
            tests_failed++; \
            printf(COLOR_RED "✗ " COLOR_RESET "%s\n", message); \
            printf(COLOR_RED "  Expected: \"%s\", Got: \"%s\" at %s:%d\n" COLOR_RESET, \
                   (expected), (actual), __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_NULL(pointer, message) \
    ASSERT_TRUE((pointer) == NULL, message)

#define ASSERT_NOT_NULL(pointer, message) \
    ASSERT_TRUE((pointer) != NULL, message)

// Test suite macros
#define TEST_SUITE_START(suite_name) \
    printf(COLOR_BLUE "\n=== Running %s ===\n" COLOR_RESET, suite_name); \
    int suite_start_tests = tests_run; \
    int suite_start_passed = tests_passed; \
    int suite_start_failed = tests_failed;

#define TEST_SUITE_END(suite_name) \
    printf(COLOR_BLUE "=== %s Results ===\n" COLOR_RESET, suite_name); \
    printf("Tests run: %d\n", tests_run - suite_start_tests); \
    printf(COLOR_GREEN "Passed: %d\n" COLOR_RESET, tests_passed - suite_start_passed); \
    if (tests_failed > suite_start_failed) { \
        printf(COLOR_RED "Failed: %d\n" COLOR_RESET, tests_failed - suite_start_failed); \
    } else { \
        printf("Failed: 0\n"); \
    }

// Function to print final test summary
void print_test_summary(void);

// Initialize test framework
void init_test_framework(void);

#endif