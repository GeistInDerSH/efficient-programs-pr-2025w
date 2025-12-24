#include "sudoku.h"

static int is_valid_unoptimized(const struct Board* board, int row, int col, uint8_t value) {
    // is unique in row
    for (int i = 0; i < 9; i++) {
        if (value == board->cells[row * 9 + i]) {
            return 0;
        }
    }

    // is unique in col
    for (int i = 0; i < 9; i++) {
        if (value == board->cells[i * 9 + col]) {
            return 0; 
        }
    }

    // is unique in box
    int box_start_col = (col / 3) * 3;
    int box_end_col = box_start_col + 3;
    int box_start_row = (row / 3) * 3;
    int box_end_row = box_start_row + 3;

    for (int r = box_start_row; r < box_end_row; r++) {
        for (int c = box_start_col; c < box_end_col; c++) {
            if (value == board->cells[r * 9 + c]) {
                return 0; 
            }
        }
    }

    return 1;
}

static int solve_recursive_unoptimized(struct Board* board, int row, int col) {
    if (row == 9) {
        return 1; // true: All rows solved
    } 

    if (col == 9) {
        return solve_recursive_unoptimized(board, row + 1, 0); // Move to next row
    } 

    if (board->cells[row * 9 + col] != 0) {
        return solve_recursive_unoptimized(board, row, col + 1); // Skip filled cell
    } 

    // Try all numbers
    for (uint8_t p = 1; p <= 9; p++) {
        if (!is_valid_unoptimized(board, row, col, p)) {
            continue;
        }
        board->cells[row * 9 + col] = p; // Make guess
        if (solve_recursive_unoptimized(board, row, col + 1)) {
            return 1; // Guess was correct
        }
        board->cells[row * 9 + col] = 0;
    }

    return 0; // No valid number found

}

int solve_unoptimized( struct Board* input, struct Board* solution) {

    // printf("solve_unoptimized\n");

    memcpy(solution, input, sizeof(struct Board));

    return solve_recursive_unoptimized(solution, 0, 0);
}