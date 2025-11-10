#ifndef SUDOKU_OPTIMIZED_V1_H
#define SUDOKU_OPTIMIZED_V1_H

#include "sudoku.h"
/*
 * Solves the Sudoku puzzle. The optimized function (v1) of the "solve" function
 * "input" is the unsolved puzzle (it is not modified).
 * "solution" is the solved Sudoku puzzle
 * Returns 1 if a solution is found, 0 if no solution exists.
 */
int solve_optimized_v1( struct Board* input, struct Board* solution);

#endif // SUDOKU_OPTIMIZED_V1_H