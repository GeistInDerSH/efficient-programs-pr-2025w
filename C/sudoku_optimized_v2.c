#include "sudoku_optimized_v2.h"
#include "sudoku.h"

static int solve_recursive_bitmask(
    struct Board* board, 
    int row, 
    int col,
    uint16_t rows[9], 
    uint16_t cols[9], 
    uint16_t boxes[9]
) {
    // Base case - success
    // If the function is called with the row = 9, this means
    // it has successfully completed the rows from 0 to 8, 
    // hence, the puzzle is solved !
    if (row == 9) {
        return 1;
    } 
    
    // Navigation login
    // From left to right and from top to bottom
    int next_row = row;
    int next_col = col + 1;
    if (next_col == 9) {
        next_row = row + 1;
        next_col = 0;
    }

    if (board->cells[row * 9 + col] != 0) {
        // The cell is already filled. Proceed to the next one
        return solve_recursive_bitmask(board, next_row, next_col, rows, cols, boxes);
    } 
    
    // The cell is empty. Try all numbers
    for (uint8_t p = 1; p <= 9; p++) {
        
        // Here is the key optimization
        // The verification takes O(1) using bitmasks
        
        const uint16_t mask = 1 << (p - 1);
        const int box_idx = (row / 3) * 3 + (col / 3);

        // Key optimization.
        if (!(rows[row] & mask) && // Checks if the bit for the number p is 0 on the current row is UNSET
            !(cols[col] & mask) && // Checks if the bit for the number p is 0 on the current column is UNSET
            !(boxes[box_idx] & mask)) // Checks if the bit for the number p is 0 on the current box is UNSET
        {
            // The number 'p' is valid. We set it.
            board->cells[row * 9 + col] = p; // Put the number on the board

            // Update the masks
            rows[row] |= mask;
            cols[col] |= mask;
            boxes[box_idx] |= mask;

            if (solve_recursive_bitmask(board, next_row, next_col, rows, cols, boxes)) {
                return 1; // Solution found
            }

            // Placing the number p led to a deadend, hence p is wrong
            // 
            board->cells[row * 9 + col] = 0;
            rows[row] &= ~mask;
            cols[col] &= ~mask;
            boxes[box_idx] &= ~mask;
        }
    }
    
    return 0;
}


int solve_optimized_v2(struct Board* input, struct Board* solution) {
    
    // E.g. rows[0] keeps the evidence of the numbers already used on the row 0
    // E.g. cols[3] keeps the evidence of the numbers already used on the column 3
    // E.g. boxes[1] keeps the evidence of the numbers already used on the box with the index 1
    uint16_t rows[9];
    uint16_t cols[9];
    uint16_t boxes[9];

    memset(rows, 0, sizeof(rows));
    memset(cols, 0, sizeof(cols));
    memset(boxes, 0, sizeof(boxes));

    memcpy(solution, input, sizeof(struct Board));

    // Set the bits in bitmasks for values that are initital present 
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            uint8_t p = solution->cells[r * 9 + c];

            // For each non-empty cell
            if (p != 0) {

                // For p = 1, mask is ...0001
                // For p = 2, mask is ...0010
                // ...
                const uint16_t mask = 1 << (p - 1);

                const int box_idx = (r / 3) * 3 + (c / 3);

                // If this number was already detected in the current row
                // OR this number was already detected in the current column
                // OR this number was already detected in the current box
                if ((rows[r] & mask) || (cols[c] & mask) || (boxes[box_idx] & mask)) {
                    // Invalid input table. Contains duplicates
                    return 0;
                }
                
                rows[r] |= mask;
                cols[c] |= mask;
                boxes[box_idx] |= mask;
            }
        }
    }

    return solve_recursive_bitmask(solution, 0, 0, rows, cols, boxes);
}