#include "sudoku_optimized_v4.h"

static int is_valid_unoptimized_UNROLLED(const struct Board* board, int row, int col, uint8_t value) {
    
    const uint8_t* cells = board->cells;
    int i = row * 9;

    // The row loop unrolling
    const uint8_t* p_row = &cells[i];
    if (p_row[0] == value) return 0;
    if (p_row[1] == value) return 0;
    if (p_row[2] == value) return 0;
    if (p_row[3] == value) return 0;
    if (p_row[4] == value) return 0;
    if (p_row[5] == value) return 0;
    if (p_row[6] == value) return 0;
    if (p_row[7] == value) return 0;
    if (p_row[8] == value) return 0;

    // The column loop unrolling
    if (cells[col + 0]  == value) return 0;
    if (cells[col + 9]  == value) return 0;
    if (cells[col + 18] == value) return 0; 
    if (cells[col + 27] == value) return 0;
    if (cells[col + 36] == value) return 0;
    if (cells[col + 45] == value) return 0;
    if (cells[col + 54] == value) return 0;
    if (cells[col + 63] == value) return 0;
    if (cells[col + 72] == value) return 0;

    // The grid loop unrolling
    int box_start_row = (row / 3) * 3;
    int box_start_col = (col / 3) * 3;
    
    i = box_start_row * 9 + box_start_col;

    // Box row 1
    if (cells[i]     == value) return 0;
    if (cells[i + 1] == value) return 0;
    if (cells[i + 2] == value) return 0;
    
    i += 9; // Go to grid row 2
    if (cells[i]     == value) return 0;
    if (cells[i + 1] == value) return 0;
    if (cells[i + 2] == value) return 0;

    i += 9; // Go to grid row 3
    if (cells[i]     == value) return 0;
    if (cells[i + 1] == value) return 0;
    if (cells[i + 2] == value) return 0;

    return 1;
}

static int solve_recursive_unoptimized_UNROLLED(struct Board* board, int row, int col) {
    if (row == 9) {
        return 1;
    } 
    else if (col == 9) {
        return solve_recursive_unoptimized_UNROLLED(board, row + 1, 0);
    } 
    else if (board->cells[row * 9 + col] != 0) {
        return solve_recursive_unoptimized_UNROLLED(board, row, col + 1);
    } 
    else {
        for (uint8_t p = 1; p <= 9; p++) {
            if (!is_valid_unoptimized_UNROLLED(board, row, col, p)) {
                continue;
            }
            board->cells[row * 9 + col] = p;
            if (solve_recursive_unoptimized_UNROLLED(board, row, col + 1)) {
                return 1;
            }
            board->cells[row * 9 + col] = 0;
        }
        return 0;
    }
}

int solve_optimized_v4( struct Board* input, struct Board* solution) {

    memcpy(solution, input, sizeof(struct Board));

    return solve_recursive_unoptimized_UNROLLED(solution, 0, 0);
}