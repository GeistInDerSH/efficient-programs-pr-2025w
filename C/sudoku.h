#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdint.h> // For uint8_t
#include <stdio.h>  // For FILE

typedef int (*SolverFunc)(struct Board*, struct Board*);

// Board is a one-dimensional array (1d) of bytes
// I used uint8_t because it is enough (the numbers from 1 to 9 will fit)
struct Board {
    uint8_t cells[81]; // 81 bytes in total
};

// Solution is an alias for Board
typedef struct Board Solution;

/*
 * Reads a Board from a file.
 * Fills the "board" struct passed as an argument.
 * Returns 0 on success, 1 on error.
 */
int read_file(struct Board* board, const char* filename);

/*
 * Solves the Sudoku puzzle.
 * "input" is the unsolved puzzle (it is not modified).
 * "solution" is the solved Sudoku puzzle
 * Returns 1 if a solution is found, 0 if no solution exists.
 */
int solve( struct Board* input, struct Board* solution);



/**
 * Prints the board 
 */
void print_board(const struct Board* board);

#endif // SUDOKU_H