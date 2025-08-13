#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ANSI escape codes for terminal colors, renamed to avoid conflicts
#define TEST_COLOR_RED     "\033[31m"
#define TEST_COLOR_GREEN   "\033[32m"
#define TEST_COLOR_BLUE    "\033[34m"
#define TEST_COLOR_RESET   "\033[0m"

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_CASE_START(name) printf(TEST_COLOR_BLUE "\n--- %s ---\n" TEST_COLOR_RESET, name)
#define TEST_CASE_END()

#define TEST_SUITE_START(name) printf(TEST_COLOR_BLUE "== %s ==\n" TEST_COLOR_RESET, name)
#define TEST_SUITE_END(name) do { \
    printf(TEST_COLOR_BLUE "\n== %s Summary ==\n" TEST_COLOR_RESET, name); \
    printf("Total tests run: %d\n", tests_run); \
    printf(TEST_COLOR_GREEN "Tests passed: %d\n" TEST_COLOR_RESET, tests_passed); \
    printf(TEST_COLOR_RED "Tests failed: %d\n" TEST_COLOR_RESET, tests_failed); \
    printf("\n"); \
} while(0)

#define ASSERT_EQ(expected,actual,message) do { \
    tests_run++; \
    if ((expected) == (actual)) { \
        tests_passed++; \
        printf(TEST_COLOR_GREEN "✓ " TEST_COLOR_RESET "%s\n", message); \
    } else { \
        tests_failed++; \
        printf(TEST_COLOR_RED "✗ " TEST_COLOR_RESET "%s\n", message); \
        printf(TEST_COLOR_RED "  Expected: %d, Got: %d at %s:%d\n" TEST_COLOR_RESET, (int)(expected), (int)(actual), __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_STR_EQ(expected, actual, message) do { \
    tests_run++; \
    if (strcmp(expected, actual) == 0) { \
        tests_passed++; \
        printf(TEST_COLOR_GREEN "✓ " TEST_COLOR_RESET "%s\n", message); \
    } else { \
        tests_failed++; \
        printf(TEST_COLOR_RED "✗ " TEST_COLOR_RESET "%s\n", message); \
        printf(TEST_COLOR_RED "  Expected: \"%s\", Got: \"%s\" at %s:%d\n" TEST_COLOR_RESET, expected, actual, __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_NOT_NULL(ptr, message) do { \
    tests_run++; \
    if (ptr != NULL) { \
        tests_passed++; \
        printf(TEST_COLOR_GREEN "✓ " TEST_COLOR_RESET "%s\n", message); \
    } else { \
        tests_failed++; \
        printf(TEST_COLOR_RED "✗ " TEST_COLOR_RESET "%s\n", message); \
        printf(TEST_COLOR_RED "  Expected not NULL, got NULL at %s:%d\n" TEST_COLOR_RESET, __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_TRUE(condition, message) do { \
    tests_run++; \
    if (condition) { \
        tests_passed++; \
        printf(TEST_COLOR_GREEN "✓ " TEST_COLOR_RESET "%s\n", message); \
    } else { \
        tests_failed++; \
        printf(TEST_COLOR_RED "✗ " TEST_COLOR_RESET "%s\n", message); \
        printf(TEST_COLOR_RED "  Condition failed: %s at %s:%d\n" TEST_COLOR_RESET, #condition, __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_FALSE(condition, message) do { \
    tests_run++; \
    if (!(condition)) { \
        tests_passed++; \
        printf(TEST_COLOR_GREEN "✓ " TEST_COLOR_RESET "%s\n", message); \
    } else { \
        tests_failed++; \
        printf(TEST_COLOR_RED "✗ " TEST_COLOR_RESET "%s\n", message); \
        printf(TEST_COLOR_RED "  Condition should be false: %s at %s:%d\n" TEST_COLOR_RESET, #condition, __FILE__, __LINE__); \
    } \
} while (0)

#define ASSERT_NULL(ptr, message) do { \
    tests_run++; \
    if (ptr == NULL) { \
        tests_passed++; \
        printf(TEST_COLOR_GREEN "✓ " TEST_COLOR_RESET "%s\n", message); \
    } else { \
        tests_failed++; \
        printf(TEST_COLOR_RED "✗ " TEST_COLOR_RESET "%s\n", message); \
        printf(TEST_COLOR_RED "  Expected NULL, got non-NULL at %s:%d\n" TEST_COLOR_RESET, __FILE__, __LINE__); \
    } \
} while (0)

#endif // TEST_FRAMEWORK_H