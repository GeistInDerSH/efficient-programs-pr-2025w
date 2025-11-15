#ifndef SUDOKU_H
#define SUDOKU_H

#include <stdint.h> // For uint8_t
#include <stdio.h>  // For FILE
#include <string.h>

struct Board;

typedef int (*SolverFunc)(struct Board*, struct Board*);

// Board is a one-dimensional array (1d) of bytes
// I used uint8_t because it is just enough (the numbers from 1 to 9 will fit)
struct Board {
    uint8_t cells[81];
};

// Solution is an alias for Board
typedef struct Board Solution;

/*
 * Reads a Board from a file.
 * Fills the "board" struct passed as an argument.
 * Returns 0 on success, 1 on error.
 */
int read_file(struct Board* board, const char* filename);

/**
 * Prints the board 
 */
void print_board(const struct Board* board);

/**
 * Prints the board in a professional manner
 */
void print_board_enhanced(const struct Board* board);

/*
* Checks if the solution is valid
*/
int is_solution_valid(const struct Board* board);

/*
* Checks if the board is valid
*/
int is_board_valid(const struct Board* board);



#endif // SUDOKU_H