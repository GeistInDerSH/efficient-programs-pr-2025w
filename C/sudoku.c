#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

static inline int is_valid(const struct Board* board, int row, int col, uint8_t value) {
    uint8_t* cells = board->cells;
    int row_offset = row * 9;

    // Check row
    for (int c = 0; c < 9; c++) {
        if (cells[row_offset + c] == value) return 0;
    }

    // Check column
    for (int r = 0; r < 9; r++) {
        if (cells[r * 9 + col] == value) return 0;
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

static int solve_recursive(struct Board* board, int row, int col) {
    if (row == 9) return 1;
    if (col == 9) return solve_recursive(board, row + 1, 0);
    if (board->cells[row * 9 + col] != 0) return solve_recursive(board, row, col + 1);

    for (uint8_t p = 1; p <= 9; p++) {
        if (!is_valid(board, row, col, p)) continue;
        board->cells[row * 9 + col] = p;
        if (solve_recursive(board, row, col + 1)) return 1;
        board->cells[row * 9 + col] = 0;
    }

    return 0;
}

int read_file(struct Board* board, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file) 
        return 1;

    int cell_index = 0;
    int c;

    while (cell_index < 81 && (c = fgetc(file)) != EOF) {
        if (c >= '0' && c <= '9') {
            board->cells[cell_index++] = (uint8_t)(c - '0');
        } else if (c != '\n' && c != '\r') {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    // Check if we really read 81 numbers ...
    return (cell_index == 81) ? 0 : 1;
}

int solve( struct Board* input, struct Board* solution) {

    // printf("solve\n");
    memcpy(solution, input, sizeof(struct Board));

    return solve_recursive(solution, 0, 0);
}



void print_board(const struct Board* board) {

    const uint8_t* cells = board->cells;

    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            putchar('0' + cells[row * 9 + col]);
        }
        putchar('\n');
    }
}

// Print board enhanced 
void print_board_enhanced(const struct Board* board) {
    const uint8_t* cells = board->cells;

    for (int row = 0; row < 9; row++) {
        // 1. Add the horiyontal seapration line
        if (row == 3 || row == 6) {
            printf("------+-------+------\n");
        }

        for (int col = 0; col < 9; col++) {
            // 2. Add the vertical separator
            if (col == 3 || col == 6) {
                printf(" | ");
            }

            // 3. PRocess and display the cell value
            uint8_t value = cells[row * 9 + col];

            if (value == 0) {
                putchar('.'); // Print '.' for the empty cells (0)
            } else {
                putchar('0' + value); // Print 1-9
            }

            // Add a space after each number (execept the separator)
            if (col != 2 && col != 5 && col != 8) {
                 putchar(' ');
            }
        }
        putchar('\n'); // Next line ...
    }
}