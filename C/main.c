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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        const char *usage = "Usage: <optimization index> <sudoku file> \n" 
        "Optimization index can be: \n"
        "0 - addition of a variable \n"
        "1 - combined loops \n"
        "2 - Bitmask \n"
        "3 - cache optimization \n"
        "4 - Loop unrolling \n"
        "5 - Lookup table \n"
        "6 - unoptimized version\n";
        fprintf(stderr, usage);
        return 1;
    }

    int optimization_index = atoi(argv[1]);

    printf("optimization_index = %d \n", optimization_index);

    const char* file_path = argv[2];

    struct Board board;
    struct Board_CacheOptimized board_cache_optimized;

    if(optimization_index == 3) { // Cache optimization - this is a special case
        if (read_file2(&board_cache_optimized, file_path) != 0) {
            fprintf(stderr, "Error reading file: %s\n", file_path);
            return 1;
        }
    }
    else if (read_file(&board, file_path) != 0) {
        fprintf(stderr, "Error reading file: %s\n", file_path);
        return 1;
    }

    Solution solution;
    Solution_CacheOptimized solution_cache_optimized;
    struct timespec start, end;

    timespec_get(&start, TIME_UTC);

    int found_solution;
    switch(optimization_index)
    {
        case 0:
        {
            found_solution = solve_optimized_v0(&board, &solution);
            break;
        }
        case 1:
        {
            found_solution = solve_optimized_v1(&board, &solution);
            break;
        }
        case 2:
        {
            found_solution = solve_optimized_v2(&board, &solution);
            break;
        }
        case 3:
        {
            found_solution = solve_optimized_v3(&board_cache_optimized, &solution_cache_optimized);
            break;
        }
        case 4:
        {
            found_solution = solve_optimized_v4(&board, &solution);
            break;
        }
        case 5:
        {
            found_solution = solve_optimized_v5(&board, &solution);
            break;
        }
        default:
        {
            found_solution = solve_unoptimized(&board, &solution);
            break;
        }
    }

    // int found_solution = solve_optimized_v0(&board, &solution);

    timespec_get(&end, TIME_UTC);
    // --- End time_it! equivalent ---

    long nanos = get_nanos(&start, &end);
    long micros = nanos / 1000;

    if (found_solution) {
        printf("Solution:\n");
        if(optimization_index == 3) {
            print_board2(&solution_cache_optimized);
        }
        else {
            print_board(&solution);
        }
        
        printf("\nTook %ldμs\n", micros);
    } else {
        printf("No solution found. Took %ldμs\n", micros);
    }

    return 0;
}