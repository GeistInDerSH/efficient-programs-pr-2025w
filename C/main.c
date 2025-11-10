#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"

// Helper to get time in nanoseconds
long get_nanos(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000000000L + (end->tv_nsec - start->tv_nsec);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No sudoku file specified\n");
        return 1;
    }
    const char* file_path = argv[1];

    struct Board board;

    if (read_file(&board, file_path) != 0) {
        fprintf(stderr, "Error reading file: %s\n", file_path);
        return 1;
    }

    Solution solution;
    struct timespec start, end;

    printf("ceva");
    clock_gettime(CLOCK_MONOTONIC, &start);
    // solve() returns 1 (Some) or 0 (None)
    int found_solution = solve(&board, &solution);

    clock_gettime(CLOCK_MONOTONIC, &end);
    // --- End time_it! equivalent ---

    long nanos = get_nanos(&start, &end);
    long micros = nanos / 1000;

    if (found_solution) {
        printf("Solution:\n");
        print_board(&solution);
        printf("\nTook %ldμs\n", micros);
    } else {
        printf("No solution found. Took %ldμs\n", micros);
    }

    return 0;
}