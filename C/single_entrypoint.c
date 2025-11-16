/**
 * This file allows for a single binary to be made, that
 * can run a single solver target potentially allowing the
 * compiler to better optimize just the single target.
 *
 * The target to use is based on which -D, e.g. -DV0
 * to use 'sudoku_optimized_v0.h'. Only a single flag
 * may be given at one time.
 *
 * Like with main.c, the runtime of the program is measured
 * however unlike main.c reading the file is included to
 * include any validation done during this step.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if (defined(UNOPTIMIZED) + \
     defined(V0) + \
     defined(V1) + \
     defined(V2) + \
     defined(V3) + \
     defined(V4) + \
     defined(V5)) != 1
  #error "You must define exactly one flag"
#endif

#include "sudoku.h"
#if defined(UNOPTIMIZED)
  #include "sudoku_unoptimized.h"
#elif defined(V0)
  #include "sudoku_optimized_v0.h"
#elif defined(V1)
  #include "sudoku_optimized_v1.h"
#elif defined(V2)
  #include "sudoku_optimized_v2.h"
#elif defined(V3)
  #include "sudoku_optimized_v3.h"
#elif defined(V4)
  #include "sudoku_optimized_v4.h"
#elif defined(V5)
  #include "sudoku_optimized_v5.h"
#else
  #error "No compiler directive was provided for which solver to use!"
#endif

long get_nanos(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000000000L + (end->tv_nsec - start->tv_nsec);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        const char *usage = "Usage: <sudoku file> \n";
        fprintf(stderr, usage);
        return 1;
    }

    const char* file_path = argv[1];

    #if defined(V3)

    // Cache optimization - this is a special case
    struct Board_CacheOptimized board_cache_optimized;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (read_file2(&board_cache_optimized, file_path) != 0) {
        fprintf(stderr, "Error reading file: %s\n", file_path);
        return 1;
    }
    Solution_CacheOptimized solution_cache_optimized;
    int found_solution = solve_optimized_v3(&board_cache_optimized, &solution_cache_optimized);
    clock_gettime(CLOCK_MONOTONIC, &end);
    long nanos = get_nanos(&start, &end);
    long micros = nanos / 1000;

    if (found_solution) {
        printf("Solution:\n");
        print_board2(&solution_cache_optimized);
        printf("\nTook %ldμs\n", micros);
    } else {
        printf("No solution found. Took %ldμs\n", micros);
    }

    #else

    // Measure the time for all other solutions that are not V3
    struct Board board;
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (read_file(&board, file_path) != 0) {
        fprintf(stderr, "Error reading file: %s\n", file_path);
        return 1;
    }
    Solution solution;

    // Determine what function to call based on which flag was passed
    // at compile time, via -D (e.g. -DV0).
    int found_solution;
    #if defined(UNOPTIMIZED)
      found_solution = solve_unoptimized(&board, &solution);
    #elif defined(V0)
      found_solution = solve_optimized_v0(&board, &solution);
    #elif defined(V1)
      found_solution = solve_optimized_v1(&board, &solution);
    #elif defined(V2)
      found_solution = solve_optimized_v2(&board, &solution);
    #elif defined(V4)
      found_solution = solve_optimized_v4(&board, &solution);
    #elif defined(V5)
      found_solution = solve_optimized_v5(&board, &solution);
    #endif

    clock_gettime(CLOCK_MONOTONIC, &end);
    long nanos = get_nanos(&start, &end);
    long micros = nanos / 1000;

    if (found_solution) {
        printf("Solution:\n");
        print_board(&solution);
        printf("\nTook %ldμs\n", micros);
    } else {
        printf("No solution found. Took %ldμs\n", micros);
    }

    #endif

    return 0;
}
