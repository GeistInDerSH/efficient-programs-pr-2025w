#ifndef SUDOKU_UNOPTIMIZED_H
#define SUDOKU_UNOPTIMIZED_H

#include "sudoku.h"
/*
 * Solves the Sudoku puzzle. The first version of the algorithm, unoptimized. Yet.
 * "input" is the unsolved puzzle (it is not modified).
 * "solution" is the solved Sudoku puzzle
 * Returns 1 if a solution is found, 0 if no solution exists.
 */
int solve_unoptimized( struct Board* input, struct Board* solution);

#endif // SUDOKU_UNOPTIMIZED_H