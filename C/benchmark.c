#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"
#include "sudoku_unoptimized.h"
#include "sudoku_optimized_v0.h"
#include "sudoku_optimized_v1.h"
#include "sudoku_optimized_v2.h"
#include "sudoku_optimized_v3.h"
#include "sudoku_optimized_v4.h"
#include "sudoku_optimized_v5.h"

// Helper to get time in nanoseconds
long get_nanos(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000000000L + (end->tv_nsec - start->tv_nsec);
}

// A very simple benchmark runner
void run_bench(const char* name, int iters, void (*func)(void*, struct Board), void* context, const char *filePath) {
    struct timespec start, end;
    long total_nanos = 0;
    
    struct Board b = {0};
    // Path updated to go up two directories
    if(read_file(&b, filePath) != 0) {
        printf("Error: Read file %s failed. Aborting the test ... \n", filePath);
        return;
    }
 
    if (is_board_valid(&b)) {
        //printf("Verification PASSED: The solver's output is correct.\n");
    } else {
        printf("Error. The board from file %s is INVALID ! Aborting the test ... \n", filePath);
        return;
    }
    
    printf("Running benchmark: %s (%d iterations)...\n", name, iters);

    for (int i = 0; i < iters; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        func(context, b); // Call the function to benchmark
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_nanos += get_nanos(&start, &end);
    }
    printf("Result: %ld ns/iter\n", total_nanos / iters);
}

// --- Benchmark Functions ---

void bench_test(void* context, struct Board b) {

    SolverFunc solver_to_run = (SolverFunc)context;

    struct Board solved_board;
    int res = solver_to_run(&b, &solved_board);

    if(res == 1)
    {
        if (is_solution_valid(&solved_board)) {
            //printf("Verification PASSED: The solver's output is correct.\n");
        // print_board(&solved_board);
        } else {
            printf("Verification FAILED: The solver produced an invalid solution!\n");
        }
    }
}



// A very simple benchmark runner
void run_bench2(const char* name, int iters, void (*func)(void*, struct Board_CacheOptimized), void* context, const char *filePath) {
    struct timespec start, end;
    long total_nanos = 0;
    
    struct Board_CacheOptimized b = {0};
    // Path updated to go up two directories
    if(read_file2(&b, filePath) != 0) {
        printf("Error: Read file %s failed. Aborting the test ... \n", filePath);
        return;
    }

    if (is_board_valid_cache_optimized(&b)) {
        //printf("Verification PASSED: The solver's output is correct.\n");
    } else {
        printf("Error. The board from file %s is INVALID ! Aborting the test ... \n", filePath);
        return;
    }

    printf("Running benchmark: %s (%d iterations)...\n", name, iters);

    for (int i = 0; i < iters; i++) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        func(context, b); // Call the function to benchmark
        clock_gettime(CLOCK_MONOTONIC, &end);
        total_nanos += get_nanos(&start, &end);
    }
    printf("Result: %ld ns/iter\n", total_nanos / iters);
}

// --- Benchmark Functions ---

void bench_test2(void* context, struct Board_CacheOptimized b) {

    SolverFuncCacheOptimized solver_to_run = (SolverFuncCacheOptimized)context;

    struct Board_CacheOptimized output;
    solver_to_run(&b, &output);
}

void bench_read_file_dne(const char *filePath) {
    struct Board b = {0};
    // We expect this to fail
    read_file(&b, filePath);
}

int main(void) {
    const int ITERS = 1; // change this by need
    

    printf("************************ Unoptimized ************************ \n");

    run_bench("Fully Solved - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/fully-solved.sudoku");
    run_bench("Fully Solved - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1 - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1 - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1 - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1 - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1 - solve_unoptimized()", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-medium-1.sudoku");


    printf("************************ \n");

    run_bench("Fully Solved - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/fully-solved.sudoku");
    run_bench("Fully Solved - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1 - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1 - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1 - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1 - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1 - solve()", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-medium-1.sudoku");
    

    printf("************************ Combined loops ************************ \n");

    run_bench("Fully Solved - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/fully-solved.sudoku");
    run_bench("Fully Solved - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1 - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1 - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1 - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1 - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1 - solve_optimized_v1()", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-medium-1.sudoku");



    printf("************************ Bitmask ************************ \n");

    run_bench("Fully Solved - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/fully-solved.sudoku");
    run_bench("Fully Solved - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1 - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1 - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1 - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1 - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1 - solve_optimized_v2()", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-medium-1.sudoku");


    printf("************************ Cache optimizations ************************ \n");

    run_bench2("Fully Solved - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/fully-solved.sudoku");
    run_bench2("Fully Solved - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-characters.sudoku");
    run_bench2("Invalid Row Col Collision - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-row-col-collision.sudoku");
    run_bench2("Solvable 2x hard - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-2x-hard.sudoku");
    run_bench2("Solvable example 1 - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-example-1.sudoku");
    run_bench2("Solvable hard 1 - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-hard-1.sudoku");
    run_bench2("Invalid Box Collision - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-box-collision.sudoku");
    run_bench2("Invalid Col Collision - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-col-collision.sudoku");
    run_bench2("Invalid Row Collision - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-row-collision.sudoku");
    run_bench2("Solvable Easy 1 - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-easy-1.sudoku");
    run_bench2("Solvable extra hard 1 - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-extra-hard-1.sudoku");
    run_bench2("Solvable medium 1 - solve_optimized_v3()", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-medium-1.sudoku");

    
    printf("************************ Loop unrolling ************************ \n");

    run_bench("Fully Solved - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/fully-solved.sudoku");
    run_bench("Fully Solved - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1 - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1 - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1 - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1 - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1 - solve_optimized_v4()", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-medium-1.sudoku");
    
        
    printf("************************ Lookup table ************************ \n");

    run_bench("Fully Solved - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/fully-solved.sudoku");
    run_bench("Fully Solved - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1 - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1 - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1 - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1 - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1 - solve_optimized_v5()", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-medium-1.sudoku");

    // run_bench("read_file dne", ITERS, bench_read_file_dne, (void*)solve, "invalid/file.sudoku");

    return 0;
}