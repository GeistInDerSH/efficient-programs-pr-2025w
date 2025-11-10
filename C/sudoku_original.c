
#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>

static int is_valid(const struct Board* board, int row, int col, uint8_t value) {
    // is unique in row
    for (int v = 0; v < 9; v++) {
        if (value == board->cells[row * 9 + v]) {
            return 0;
        }
    }

    // is unique in col
    for (int v = 0; v < 9; v++) {
        if (value == board->cells[v * 9 + col]) {
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

static int solve_recursive(struct Board* board, int row, int col) {
    if (row == 9) {
        return 1; // true: All rows solved
    } else if (col == 9) {
        return solve_recursive(board, row + 1, 0); // Move to next row
    } else if (board->cells[row * 9 + col] != 0) {
        return solve_recursive(board, row, col + 1); // Skip filled cell
    } else {
        // Try all numbers
        for (uint8_t p = 1; p <= 9; p++) {
            if (!is_valid(board, row, col, p)) {
                continue;
            }
            board->cells[row * 9 + col] = p; // Make guess
            if (solve_recursive(board, row, col + 1)) {
                return 1; // Guess was correct
            }
            board->cells[row * 9 + col] = 0;
        }
        return 0; // No valid number found
    }
}

// --- Public API Functions (defined in sudoku.h) ---

int read_file(struct Board* board, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        return 1; // Error
    }

    int cell_index = 0;
    int c;
    while (cell_index < 81 && (c = fgetc(file)) != EOF) {
        if (c >= '0' && c <= '9') {
            board->cells[cell_index] = (uint8_t)(c - '0');
            cell_index++;
        } else if (c == '\n' || c == '\r') {
            continue; // Skip newlines
        } else {
            // Invalid character
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return (cell_index == 81) ? 0 : 1; // 0 (Success) if we read 81 numbers
}

int solve(struct Board* input, struct Board* solution) {
    // Copy input to solution, then solve solution in-place
    *solution = *input; // This is a full struct copy
    return solve_recursive(solution, 0, 0);
}

int solve_optimized(struct Board* input, struct Board* solution) {
    // Copy input to solution, then solve solution in-place
    *solution = *input; // This is a full struct copy

}

// 'Display' impl for 'Board'
void print_board(const struct Board* board) {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            printf("%d", board->cells[row * 9 + col]);
        }
        printf("\n");
    }
}