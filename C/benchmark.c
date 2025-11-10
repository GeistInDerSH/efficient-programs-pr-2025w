#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"
#include "sudoku_optimized_v1.h"

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
        printf("Read file %s failed\n", filePath);
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

    // struct Board b = {0};
    // // Path updated to go up two directories
    // if(read_file(&b, filePath) != 0) {
    //     printf("Read file %s failed\n", filePath);
    //     return;
    // }

    struct Board output;
    solver_to_run(&b, &output);

    // print_board(&output);
}




void bench_read_file_dne(const char *filePath) {
    struct Board b = {0};
    // We expect this to fail
    read_file(&b, filePath);
}

int main(void) {
    const int ITERS = 1000;
    
    run_bench("Fully Solved", ITERS, bench_test, (void*)solve, "../boards/fully-solved.sudoku");
    // run_bench("Fully Solved", ITERS, bench_test, (void*)solve, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision", ITERS, bench_test, (void*)solve, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard", ITERS, bench_test, (void*)solve, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1", ITERS, bench_test, (void*)solve, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1", ITERS, bench_test, (void*)solve, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision", ITERS, bench_test, (void*)solve, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision", ITERS, bench_test, (void*)solve, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision", ITERS, bench_test, (void*)solve, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1", ITERS, bench_test, (void*)solve, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1", ITERS, bench_test, (void*)solve, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1", ITERS, bench_test, (void*)solve, "../boards/solvable-medium-1.sudoku");
    

    printf("************************ \n");

    run_bench("Fully Solved", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/fully-solved.sudoku");
    // run_bench("Fully Solved", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-characters.sudoku");
    run_bench("Invalid Row Col Collision", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-row-col-collision.sudoku");
    run_bench("Solvable 2x hard", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-2x-hard.sudoku");
    run_bench("Solvable example 1", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-example-1.sudoku");
    run_bench("Solvable hard 1", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-hard-1.sudoku");
    run_bench("Invalid Box Collision", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-box-collision.sudoku");
    run_bench("Invalid Col Collision", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-col-collision.sudoku");
    run_bench("Invalid Row Collision", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/invalid-row-collision.sudoku");
    run_bench("Solvable Easy 1", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-easy-1.sudoku");
    run_bench("Solvable extra hard 1", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-extra-hard-1.sudoku");
    run_bench("Solvable medium 1", ITERS, bench_test, (void*)solve_optimized_v1, "../boards/solvable-medium-1.sudoku");

    
    
    
    
    // run_bench("read_file dne", ITERS, bench_read_file_dne, (void*)solve, "invalid/file.sudoku");

    return 0;
}