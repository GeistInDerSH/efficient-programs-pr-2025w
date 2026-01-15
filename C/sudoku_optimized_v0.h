#ifndef SUDOKU_OPTIMIZED_V0_H
#define SUDOKU_OPTIMIZED_V0_H

#include "sudoku.h"
/*
 * Solves the Sudoku puzzle.
 * "input" is the unsolved puzzle (it is not modified).
 * "solution" is the solved Sudoku puzzle
 * Returns 1 if a solution is found, 0 if no solution exists.
 */
int solve_optimized_v0( struct Board* input, struct Board* solution);

#endif // SUDOKU_OPTIMIZED_V0_H