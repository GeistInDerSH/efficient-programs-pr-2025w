#include "sudoku_optimized_v0.h"

static inline int is_valid(const struct Board* board, int row, int col, uint8_t value) {
    int row_offset = row * 9;

    // Check row
    for (int c = 0; c < 9; c++) {
        if (board->cells[row_offset + c] == value)
            return 0;
    }

    // Check column
    for (int r = 0; r < 9; r++) {
        if (board->cells[r * 9 + col] == value)
            return 0;
    }

    // Check 3x3 box
    int box_row_start = (row / 3) * 3;
    int box_col_start = (col / 3) * 3;

    for (int r = 0; r < 3; r++) {
        int row_idx = (box_row_start + r) * 9;
        for (int c = 0; c < 3; c++) {
            if (board->cells[row_idx + box_col_start + c] == value)
                return 0;
        }
    }

    // if we are here, then our selection was good
    return 1;
}

static int solve_recursive(struct Board* board, int row, int col) {

    // If we are past the last row, then everythign is consistently filled -> Sudoku is solved
    if (row == 9) {
        return 1;
    }

    // We reached the end of a row -> Move to the next row
    if (col == 9) {
        return solve_recursive(board, row + 1, 0);
    }

    // Skip the currently filled cells
    if (board->cells[row * 9 + col] != 0) {
        return solve_recursive(board, row, col + 1);
    }

    // The backtracking magic
    for (uint8_t p = 1; p <= 9; p++) {

        // Check if the number "p" breaks the Sudoku rules
        if (!is_valid(board, row, col, p)) {
            continue;
        }

        // make a try
        board->cells[row * 9 + col] = p;

        // recurse
        if (solve_recursive(board, row, col + 1)) {
            return 1; // propagate the success upward
        }

        // Recursion failed -> Undo placement
        board->cells[row * 9 + col] = 0;
    }

    return 0;
}

int solve_optimized_v0( struct Board* input, struct Board* solution) {

    memcpy(solution, input, sizeof(struct Board));

    return solve_recursive(solution, 0, 0);
}
