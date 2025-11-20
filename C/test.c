#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "sudoku.h"
#include "sudoku_unoptimized.h"
#include "sudoku_optimized_v0.h"
#include "sudoku_optimized_v1.h"
#include "sudoku_optimized_v2.h"
#include "sudoku_optimized_v3.h"
#include "sudoku_optimized_v4.h"
#include "sudoku_optimized_v5.h"

typedef int (*solve_function)(struct Board *, struct Board *);
typedef int (*solve_function_cache_optimized)(struct Board_CacheOptimized *, struct Board_CacheOptimized *);

typedef struct test_entry {
    const char *test_name;
    const char *file_name;
} test_case;

const int INVALID_BOARD_COUNT = 5;
const test_case INVALID_BOARDS[] = {
    {.test_name = "Invalid Characters        ", .file_name = "../boards/invalid-characters.sudoku"},
    {.test_name = "Invalid Box Collision     ", .file_name = "../boards/invalid-box-collision.sudoku"},
    {.test_name = "Invalid Col Collision     ", .file_name = "../boards/invalid-col-collision.sudoku"},
    {.test_name = "Invalid Row Col Collision ", .file_name = "../boards/invalid-row-col-collision.sudoku"},
    {.test_name = "Invalid Row Collision     ", .file_name = "../boards/invalid-row-collision.sudoku"},
};

const int SOLVABLE_BOARD_COUNT = 7;
const test_case SOLVABLE_BOARDS[] = {
    {.test_name = "Fully Solved              ", .file_name = "../boards/fully-solved.sudoku"},
    {.test_name = "Solvable 2x hard          ", .file_name = "../boards/solvable-2x-hard.sudoku"},
    {.test_name = "Solvable Easy 1           ", .file_name = "../boards/solvable-easy-1.sudoku"},
    {.test_name = "Solvable example 1        ", .file_name = "../boards/solvable-example-1.sudoku"},
    {.test_name = "Solvable extra hard 1     ", .file_name = "../boards/solvable-extra-hard-1.sudoku"},
    {.test_name = "Solvable hard 1           ", .file_name = "../boards/solvable-hard-1.sudoku"},
    {.test_name = "Solvable medium 1         ", .file_name = "../boards/solvable-medium-1.sudoku"},
};

bool test_solvable(const char *file_name, solve_function solve_fn) {
    struct Board board = {0};
    if(read_file(&board, file_name) != 0) {
        return false;
    }
    struct Board solution = {0};
    return solve_fn(&board, &solution) == 1 && is_solution_valid(&solution);
}

bool test_invalid(const char *file_name, solve_function solve_fn) {
    struct Board board = {0};
    if(read_file(&board, file_name) != 0) {
        return true;
    }
    struct Board solution = {0};
    return solve_fn(&board, &solution) == 0;
}

bool run_tests(const char *group_name, solve_function solve_fn) {
    bool all_tests_pass = true;
    puts(group_name);

    // Run tests for invalid input
    for (int i = 0; i < INVALID_BOARD_COUNT; i += 1) {
        const test_case test = INVALID_BOARDS[i];
        if (test_invalid(test.file_name, solve_fn)) {
            printf("%s: PASS\n", test.test_name);
        } else {
            printf("%s: FAIL\n", test.test_name);
            all_tests_pass = false;
        }
    }

    // Run tests for valid input
    for (int i = 0; i < SOLVABLE_BOARD_COUNT; i += 1) {
        const test_case test = SOLVABLE_BOARDS[i];
        if (test_solvable(test.file_name, solve_fn)) {
            printf("%s: PASS\n", test.test_name);
        } else {
            printf("%s: FAIL\n", test.test_name);
            all_tests_pass = false;
        }
    }

    puts("");

    return all_tests_pass;
}

bool test_solvable_cache_optimized(const char *file_name, solve_function_cache_optimized solve_fn) {
    struct Board_CacheOptimized board = {0};
    if(read_file2(&board, file_name) != 0) {
        return false;
    }
    struct Board_CacheOptimized solution = {0};
    return solve_fn(&board, &solution) == 0 && is_board_valid_cache_optimized(&solution);
}

bool test_invalid_cache_optimized(const char *file_name, solve_function_cache_optimized solve_fn) {
    struct Board_CacheOptimized board = {0};
    if(read_file2(&board, file_name) != 0) {
        return true;
    }
    struct Board_CacheOptimized solution = {0};
    return solve_fn(&board, &solution) == 0;
}

bool run_tests_cache_optimized(const char *group_name, solve_function_cache_optimized solve_fn) {
    bool all_tests_pass = true;
    puts(group_name);

    // Run tests for invalid input
    for (int i = 0; i < INVALID_BOARD_COUNT; i += 1) {
        const test_case test = INVALID_BOARDS[i];
        if (test_invalid_cache_optimized(test.file_name, solve_fn)) {
            printf("%s: PASS\n", test.test_name);
        } else {
            printf("%s: FAIL\n", test.test_name);
            all_tests_pass = false;
        }
    }

    // Run tests for valid input
    for (int i = 0; i < SOLVABLE_BOARD_COUNT; i += 1) {
        const test_case test = SOLVABLE_BOARDS[i];
        if (test_solvable_cache_optimized(test.file_name, solve_fn)) {
            printf("%s: PASS\n", test.test_name);
        } else {
            printf("%s: FAIL\n", test.test_name);
            all_tests_pass = false;
        }
    }
    puts("");

    return all_tests_pass;
}

int main(void) {
    bool all_tests_pass = true;
    all_tests_pass &= run_tests("solve_unoptimized()", solve_unoptimized);
    all_tests_pass &= run_tests("solve_optimized_v0()", solve_optimized_v0);
    all_tests_pass &= run_tests("solve_optimized_v1()", solve_optimized_v1);
    all_tests_pass &= run_tests("solve_optimized_v2()", solve_optimized_v2);
    all_tests_pass &= run_tests_cache_optimized("solve_optimized_v3()", solve_optimized_v3);
    all_tests_pass &= run_tests("solve_optimized_v4()", solve_optimized_v4);
    all_tests_pass &= run_tests("solve_optimized_v5()", solve_optimized_v5);
    return all_tests_pass ? 0 : 1;
}