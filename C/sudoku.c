#include "sudoku.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

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

void print_board(const struct Board* board) {

    const uint8_t* cells = board->cells;

    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            putchar('0' + cells[row * 9 + col]);
        }
        putchar('\n');
    }
}

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
        putchar('\n'); // PRoceed to the next line ...
    }
}

int is_solution_valid(const struct Board* board) {
    
    // 'seen' arrays act as a "Set" for each group.
    // We need 9 checkers for rows, 9 for columns, and 9 for boxes.
    // Size 10 to use the number's value (1-9) directly as an index.
    uint8_t seen_rows[9][10] = {0};
    uint8_t seen_cols[9][10] = {0};
    uint8_t seen_boxes[9][10] = {0};

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            
            uint8_t value = board->cells[r * 9 + c];

            // 1. Check for Completeness and Valid Range
            // A valid, solved board should *only* contain numbers 1-9.
            if (value == 0 || value > 9) {
                // printf("Validation Error: Cell (%d,%d) is empty or invalid.\n", r, c);
                return 0; // Invalid: Solution is incomplete or corrupt
            }

            // 2. Check Row
            if (seen_rows[r][value]) {
                // printf("Validation Error: Duplicate %u in row %d.\n", value, r);
                return 0; // Invalid: Duplicate in row
            }
            seen_rows[r][value] = 1;

            // 3. Check Column
            if (seen_cols[c][value]) {
                // printf("Validation Error: Duplicate %u in col %d.\n", value, c);
                return 0; // Invalid: Duplicate in column
            }
            seen_cols[c][value] = 1;

            // 4. Check Box
            int box_idx = (r / 3) * 3 + (c / 3);
            if (seen_boxes[box_idx][value]) {
                // printf("Validation Error: Duplicate %u in box %d.\n", value, box_idx);
                return 0; // Invalid: Duplicate in box
            }
            seen_boxes[box_idx][value] = 1;
        }
    }

    // If we looped through all 81 cells and found no errors,
    // the solution is valid and complete.
    return 1; // Valid
}

int is_board_valid(const struct Board* board) {

    // seen_rows[r][num] == 1 means that the number 'num' has been spotten on the row 'r'
    uint8_t seen_rows[9][10] = {0};
    uint8_t seen_cols[9][10] = {0};
    uint8_t seen_boxes[9][10] = {0};

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            
            uint8_t value = board->cells[r * 9 + c];

            // Ignore the empty cells
            if (value == 0) {
                continue;
            }

            // Check the row
            if (seen_rows[r][value]) {
                return 0; // Invalid: Duplicate in this row
            }
            seen_rows[r][value] = 1; // Mark it as "seen"

            // Check the column
            if (seen_cols[c][value]) {
                return 0; // Invalid: Duplicate in this column 
            }
            seen_cols[c][value] = 1;  // Mark it as "seen"

            // Check the 3x3 grid
            int box_idx = (r / 3) * 3 + (c / 3);
            if (seen_boxes[box_idx][value]) {
                return 0; // Invalid: Duplicate in this grid 
            }
            seen_boxes[box_idx][value] = 1;  // Mark it as "seen"
        }
    }

    return 1; // Valid if we reach this point
}