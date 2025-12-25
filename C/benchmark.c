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
void benchmark_solver(const char* name, int iters, void (*func)(void*, struct Board), void* context, const char *filePath) {
    struct timespec start, end;
    long total_nanos = 0;
    
    struct Board b = {0};
    // Path updated to go up two directories
    if(read_file(&b, filePath) != 0) {
        return;
    }
 
    if (!is_board_valid(&b)) {
        return;
    }

    for (int i = 0; i < iters; i++) {
        timespec_get(&start, TIME_UTC);
        func(context, b); // Call the function to benchmark
        timespec_get(&end, TIME_UTC);
        total_nanos += get_nanos(&start, &end);
    }
    printf("Solver Only - %s: %ld ns/iter\n", name, total_nanos / iters);
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

void benchmark_full_run(const char* name, int iters, void (*func)(void*, struct Board), void* context, const char *filePath) {
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    for (int i = 0; i < iters; i++) {
        struct Board b = {0};
        // Path updated to go up two directories
        if(read_file(&b, filePath) != 0) {
            continue;
        }
        if (!is_board_valid(&b)) {
            continue;
        }
        func(context, b); // Call the function to benchmark
    }
    timespec_get(&end, TIME_UTC);
    long total_nanos = get_nanos(&start, &end);
    printf("Full Run    - %s: %ld ns/iter\n", name, total_nanos / iters);
}

void benchmark_runner(const char* name, int iters, void (*func)(void*, struct Board), void* context, const char *filePath) {
    benchmark_solver(name, iters, func, context, filePath);
    benchmark_full_run(name, iters, func, context, filePath);
}

// A very simple benchmark runner
void benchmark_solver_cache_optimized(const char* name, int iters, void (*func)(void*, struct Board_CacheOptimized), void* context, const char *filePath) {
    struct timespec start, end;
    long total_nanos = 0;
    
    struct Board_CacheOptimized b = {0};
    // Path updated to go up two directories
    if(read_file2(&b, filePath) != 0) {
        return;
    }

    if (!is_board_valid_cache_optimized(&b)) {
        return;
    }

    for (int i = 0; i < iters; i++) {
        timespec_get(&start, TIME_UTC);
        func(context, b); // Call the function to benchmark
        timespec_get(&end, TIME_UTC);
        total_nanos += get_nanos(&start, &end);
    }
    printf("Solver Only - %s: %ld ns/iter\n", name, total_nanos / iters);
}

void benchmark_full_run_cache_optimized(const char* name, int iters, void (*func)(void*, struct Board_CacheOptimized), void* context, const char *filePath) {
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);


    for (int i = 0; i < iters; i++) {
        struct Board_CacheOptimized b = {0};
        if(read_file2(&b, filePath) != 0) {
            continue;
        }
        if (!is_board_valid_cache_optimized(&b)) {
            continue;
        }
        func(context, b);
    }
    timespec_get(&end, TIME_UTC);
    long total_nanos = get_nanos(&start, &end);
    printf("Full Run    - %s: %ld ns/iter\n", name, total_nanos / iters);
}

void benchmark_runner_cache_optimized(const char* name, int iters, void (*func)(void*, struct Board_CacheOptimized), void* context, const char *filePath) {
    benchmark_solver_cache_optimized(name, iters, func, context, filePath);
    benchmark_full_run_cache_optimized(name, iters, func, context, filePath);
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
    const int ITERS = 100; // change this by need
    

    printf("************************ Unoptimized ************************ \n");

    puts("solve_unoptimized()");
    benchmark_runner("Fully Solved              ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/fully-solved.sudoku");
    benchmark_runner("Invalid Characters        ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-characters.sudoku");
    benchmark_runner("Invalid Box Collision     ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-box-collision.sudoku");
    benchmark_runner("Invalid Col Collision     ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-col-collision.sudoku");
    benchmark_runner("Invalid Row Col Collision ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-row-col-collision.sudoku");
    benchmark_runner("Invalid Row Collision     ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/invalid-row-collision.sudoku");
    benchmark_runner("Solvable 2x hard          ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-2x-hard.sudoku");
    benchmark_runner("Solvable Easy 1           ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-easy-1.sudoku");
    benchmark_runner("Solvable example 1        ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-example-1.sudoku");
    benchmark_runner("Solvable extra hard 1     ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-extra-hard-1.sudoku");
    benchmark_runner("Solvable hard 1           ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-hard-1.sudoku");
    benchmark_runner("Solvable medium 1         ", ITERS, bench_test, (void*)solve_unoptimized, "../boards/solvable-medium-1.sudoku");


    printf("************************ Single variable added ************************ \n");

    puts("solve_optimized_v0()");
    benchmark_runner("Fully Solved              ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/fully-solved.sudoku");
    benchmark_runner("Invalid Characters        ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-characters.sudoku");
    benchmark_runner("Invalid Box Collision     ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-box-collision.sudoku");
    benchmark_runner("Invalid Col Collision     ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-col-collision.sudoku");
    benchmark_runner("Invalid Row Col Collision ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-row-col-collision.sudoku");
    benchmark_runner("Invalid Row Collision     ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/invalid-row-collision.sudoku");
    benchmark_runner("Solvable 2x hard          ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-2x-hard.sudoku");
    benchmark_runner("Solvable Easy 1           ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-easy-1.sudoku");
    benchmark_runner("Solvable example 1        ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-example-1.sudoku");
    benchmark_runner("Solvable extra hard 1     ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-extra-hard-1.sudoku");
    benchmark_runner("Solvable hard 1           ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-hard-1.sudoku");
    benchmark_runner("Solvable medium 1         ", ITERS, bench_test, (void*)solve_optimized_v0, "../boards/solvable-medium-1.sudoku");


    printf("************************ Combined loops ************************ \n");

    puts("solve_optimized_v1()");
    benchmark_runner("Fully Solved              ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/fully-solved.sudoku");
    benchmark_runner("Invalid Characters        ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-characters.sudoku");
    benchmark_runner("Invalid Box Collision     ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-box-collision.sudoku");
    benchmark_runner("Invalid Col Collision     ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-col-collision.sudoku");
    benchmark_runner("Invalid Row Col Collision ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-row-col-collision.sudoku");
    benchmark_runner("Invalid Row Collision     ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-row-collision.sudoku");
    benchmark_runner("Solvable 2x hard          ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-2x-hard.sudoku");
    benchmark_runner("Solvable Easy 1           ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-easy-1.sudoku");
    benchmark_runner("Solvable example 1        ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-example-1.sudoku");
    benchmark_runner("Solvable extra hard 1     ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-extra-hard-1.sudoku");
    benchmark_runner("Solvable hard 1           ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-hard-1.sudoku");
    benchmark_runner("Solvable medium 1         ", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-medium-1.sudoku");



    printf("************************ Bitmask ************************ \n");

    puts("solve_optimized_v2()");
    benchmark_runner("Fully Solved              ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/fully-solved.sudoku");
    benchmark_runner("Invalid Characters        ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-characters.sudoku");
    benchmark_runner("Invalid Box Collision     ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-box-collision.sudoku");
    benchmark_runner("Invalid Col Collision     ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-col-collision.sudoku");
    benchmark_runner("Invalid Row Col Collision ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-row-col-collision.sudoku");
    benchmark_runner("Invalid Row Collision     ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/invalid-row-collision.sudoku");
    benchmark_runner("Solvable 2x hard          ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-2x-hard.sudoku");
    benchmark_runner("Solvable Easy 1           ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-easy-1.sudoku");
    benchmark_runner("Solvable example 1        ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-example-1.sudoku");
    benchmark_runner("Solvable extra hard 1     ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-extra-hard-1.sudoku");
    benchmark_runner("Solvable hard 1           ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-hard-1.sudoku");
    benchmark_runner("Solvable medium 1         ", ITERS, bench_test, (void*)solve_optimized_v2, "../boards/solvable-medium-1.sudoku");


    printf("************************ Cache optimizations ************************ \n");

    puts("solve_optimized_v3()");
    benchmark_runner_cache_optimized("Fully Solved              ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/fully-solved.sudoku");
    benchmark_runner_cache_optimized("Invalid Characters        ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-characters.sudoku");
    benchmark_runner_cache_optimized("Invalid Box Collision     ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-box-collision.sudoku");
    benchmark_runner_cache_optimized("Invalid Col Collision     ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-col-collision.sudoku");
    benchmark_runner_cache_optimized("Invalid Row Col Collision ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-row-col-collision.sudoku");
    benchmark_runner_cache_optimized("Invalid Row Collision     ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/invalid-row-collision.sudoku");
    benchmark_runner_cache_optimized("Solvable 2x hard          ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-2x-hard.sudoku");
    benchmark_runner_cache_optimized("Solvable Easy 1           ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-easy-1.sudoku");
    benchmark_runner_cache_optimized("Solvable example 1        ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-example-1.sudoku");
    benchmark_runner_cache_optimized("Solvable extra hard 1     ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-extra-hard-1.sudoku");
    benchmark_runner_cache_optimized("Solvable hard 1           ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-hard-1.sudoku");
    benchmark_runner_cache_optimized("Solvable medium 1         ", ITERS, bench_test2, (void*)solve_optimized_v3, "../boards/solvable-medium-1.sudoku");

    
    printf("************************ Loop unrolling ************************ \n");

    puts("solve_optimized_v4()");
    benchmark_runner("Fully Solved              ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/fully-solved.sudoku");
    benchmark_runner("Invalid Characters        ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-characters.sudoku");
    benchmark_runner("Invalid Box Collision     ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-box-collision.sudoku");
    benchmark_runner("Invalid Col Collision     ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-col-collision.sudoku");
    benchmark_runner("Invalid Row Col Collision ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-row-col-collision.sudoku");
    benchmark_runner("Invalid Row Collision     ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/invalid-row-collision.sudoku");
    benchmark_runner("Solvable 2x hard          ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-2x-hard.sudoku");
    benchmark_runner("Solvable Easy 1           ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-easy-1.sudoku");
    benchmark_runner("Solvable example 1        ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-example-1.sudoku");
    benchmark_runner("Solvable extra hard 1     ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-extra-hard-1.sudoku");
    benchmark_runner("Solvable hard 1           ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-hard-1.sudoku");
    benchmark_runner("Solvable medium 1         ", ITERS, bench_test, (void*)solve_optimized_v4, "../boards/solvable-medium-1.sudoku");

        
    printf("************************ Lookup table ************************ \n");

    puts("solve_optimized_v5()");
    benchmark_runner("Fully Solved              ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/fully-solved.sudoku");
    benchmark_runner("Invalid Characters        ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-characters.sudoku");
    benchmark_runner("Invalid Box Collision     ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-box-collision.sudoku");
    benchmark_runner("Invalid Col Collision     ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-col-collision.sudoku");
    benchmark_runner("Invalid Row Col Collision ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-row-col-collision.sudoku");
    benchmark_runner("Invalid Row Collision     ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/invalid-row-collision.sudoku");
    benchmark_runner("Solvable 2x hard          ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-2x-hard.sudoku");
    benchmark_runner("Solvable Easy 1           ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-easy-1.sudoku");
    benchmark_runner("Solvable example 1        ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-example-1.sudoku");
    benchmark_runner("Solvable extra hard 1     ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-extra-hard-1.sudoku");
    benchmark_runner("Solvable hard 1           ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-hard-1.sudoku");
    benchmark_runner("Solvable medium 1         ", ITERS, bench_test, (void*)solve_optimized_v5, "../boards/solvable-medium-1.sudoku");

    return 0;
}