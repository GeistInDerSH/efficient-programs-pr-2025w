#include "sudoku_optimized_v1.h"


static int is_valid_combined(const struct Board* board, int row, int col, uint8_t value) {
    const uint8_t* cells = board->cells;
    int row_offset = row * 9;

    // The combined loop
    // We check the row and the column in the same loop
    for (int i = 0; i < 9; i++) {
        // Check the row
        if (cells[row_offset + i] == value) {
            return 0;
        }
        
        // Check the column
        if (cells[i * 9 + col] == value) {
            return 0;
        }
    }

    // Check 3x3 box
    int box_row_start = (row / 3) * 3;
    int box_col_start = (col / 3) * 3;

    for (int r = 0; r < 3; r++) {
        int row_idx = (box_row_start + r) * 9;
        for (int c = 0; c < 3; c++) {
            if (cells[row_idx + box_col_start + c] == value) return 0;
        }
    }
    
    return 1;
}

static int solve_recursive_combined(struct Board* board, int row, int col) {
    if (row == 9) {
        return 1;
    } 
    else if (col == 9) {
        return solve_recursive_combined(board, row + 1, 0);
    } 
    else if (board->cells[row * 9 + col] != 0) {
        return solve_recursive_combined(board, row, col + 1);
    } 
    else {
        for (uint8_t p = 1; p <= 9; p++) {
            
            if (!is_valid_combined(board, row, col, p)) {
                continue;
            }
            
            board->cells[row * 9 + col] = p;
            if (solve_recursive_combined(board, row, col + 1)) {
                return 1;
            }
            board->cells[row * 9 + col] = 0;
        }

        return 0;
    }
}


int solve_optimized_v1(struct Board* input, struct Board* solution) {

    memcpy(solution, input, sizeof(struct Board));

    return solve_recursive_combined(solution, 0, 0);
}
