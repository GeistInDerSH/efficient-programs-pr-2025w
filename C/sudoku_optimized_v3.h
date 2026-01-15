#ifndef SUDOKU_OPTIMIZED_V3_H
#define SUDOKU_OPTIMIZED_V3_H

#include <stdint.h> 
#include <stdio.h> 
#include <string.h>

struct Board_CacheOptimized {
    // Normal storage, row by row
    uint8_t cells_row_major[81];
    
    // Transposed storage, column by column
    uint8_t cells_col_major[81];
};

typedef int (*SolverFuncCacheOptimized)(struct Board_CacheOptimized*, struct Board_CacheOptimized*);

// Solution_CacheOptimized is an alias for Board_CacheOptimized
typedef struct Board_CacheOptimized Solution_CacheOptimized;


/*
 * Reads a Board_CacheOptimized from a file.
 * Fills the "board" struct passed as an argument.
 * Returns 0 on success, 1 on error.
 */
int read_file2(struct Board_CacheOptimized* board, const char* filename);

int is_board_valid_cache_optimized(const struct Board_CacheOptimized* board);

/*
 * Solves the Sudoku puzzle.
 * "input" is the unsolved puzzle (it is not modified).
 * "solution" is the solved Sudoku puzzle
 * Returns 1 if a solution is found, 0 if no solution exists.
 */
int solve_optimized_v3( struct Board_CacheOptimized* input, struct Board_CacheOptimized* solution);


/**
 * Prints the board 
 */
void print_board2(const struct Board_CacheOptimized* board);


/*
* Checks if the solution is valid
*/
int is_solution_valid2(const struct Board_CacheOptimized* board);

/*
* Checks if the board is valid
*/
int is_board_valid2(const struct Board_CacheOptimized* board);


#endif // SUDOKU_OPTIMIZED_V3_H