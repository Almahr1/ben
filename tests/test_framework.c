#include "test_framework.h"

// Global test counters
int tests_run = 0;
int tests_passed = 0;
int tests_failed = 0;

void init_test_framework(void) {
    tests_run = 0;
    tests_passed = 0;
    tests_failed = 0;
    printf(COLOR_BLUE "Initializing Test Framework...\n" COLOR_RESET);
}

void print_test_summary(void) {
    printf(COLOR_BLUE "\n=== FINAL TEST SUMMARY ===\n" COLOR_RESET);
    printf("Total tests run: %d\n", tests_run);
    printf(COLOR_GREEN "Total passed: %d\n" COLOR_RESET, tests_passed);
    
    if (tests_failed > 0) {
        printf(COLOR_RED "Total failed: %d\n" COLOR_RESET, tests_failed);
        printf(COLOR_RED "SUCCESS RATE: %.1f%%\n" COLOR_RESET, 
               (double)tests_passed / tests_run * 100);
    } else {
        printf("Total failed: 0\n");
        printf(COLOR_GREEN "SUCCESS RATE: 100.0%%\n" COLOR_RESET);
        printf(COLOR_GREEN "🎉 All tests passed!\n" COLOR_RESET);
    }
}