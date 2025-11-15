#include "sudoku_optimized_v5.h"

// This table maps a row (0-8) to the starding row of its grid
static const uint8_t g_box_start_lookup[9] = {
    0, 0, 0, // Rows 0, 1, 2 -> Begin at 0
    3, 3, 3, // Rows 3, 4, 5 -> Begin at 3
    6, 6, 6  // Rows 6, 7, 8 -> Begin at 6
};

static int is_valid_unoptimized_LOOKUP(const struct Board* board, int row, int col, uint8_t value) {
    
    for (int v = 0; v < 9; v++) {
        if (board->cells[row * 9 + v] == value) {
            return 0;
        }
    }

    for (int v = 0; v < 9; v++) {
        if (board->cells[v * 9 + col] == value) {
            return 0; 
        }
    }

    // We replace the DIV and MUL with 2 memory read 
    int box_start_row = g_box_start_lookup[row];
    int box_start_col = g_box_start_lookup[col];

    int box_end_row = box_start_row + 3;
    int box_end_col = box_start_col + 3;

    for (int r = box_start_row; r < box_end_row; r++) {
        for (int c = box_start_col; c < box_end_col; c++) {
            if (board->cells[r * 9 + c] == value) {
                return 0; 
            }
        }
    }

    return 1;
}

static int solve_recursive_unoptimized_LOOKUP(struct Board* board, int row, int col) {
    if (row == 9) {
        return 1;
    } 
    else if (col == 9) {
        return solve_recursive_unoptimized_LOOKUP(board, row + 1, 0);
    } 
    else if (board->cells[row * 9 + col] != 0) {
        return solve_recursive_unoptimized_LOOKUP(board, row, col + 1);
    } 
    else {
        for (uint8_t p = 1; p <= 9; p++) {
            if (!is_valid_unoptimized_LOOKUP(board, row, col, p)) {
                continue;
            }
            board->cells[row * 9 + col] = p;
            if (solve_recursive_unoptimized_LOOKUP(board, row, col + 1)) {
                return 1;
            }
            board->cells[row * 9 + col] = 0;
        }
        return 0;
    }
}

int solve_optimized_v5( struct Board* input, struct Board* solution) {

    memcpy(solution, input, sizeof(struct Board));

    return solve_recursive_unoptimized_LOOKUP(solution, 0, 0);
}