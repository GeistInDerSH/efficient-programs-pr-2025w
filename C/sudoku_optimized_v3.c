#include "sudoku_optimized_v3.h"


static int is_valid_cache_friendly(const struct Board_CacheOptimized* board, 
                                     int row, int col, uint8_t value) 
{
    const uint8_t* row_data = &board->cells_row_major[row * 9];
    const uint8_t* col_data = &board->cells_col_major[col * 9];

    // Combined loop
    for (int v = 0; v < 9; v++) {
        
        // Check the raw (sequential access)
        // Access row_data[0], row_data[1], row_data[2]...
        if (row_data[v] == value) {
            return 0;
        }
        
        // Check the column (sequential access too)
        // Access col_data[0], col_data[1], col_data[2]...
        // (Which are the cells (0,col), (1,col), (2,col)...)
        if (col_data[v] == value) {
            return 0;
        }
    }

    int box_start_row = (row / 3) * 3;
    int box_start_col = (col / 3) * 3;
    for (int r = box_start_row; r < box_start_row + 3; r++) {
        for (int c = box_start_col; c < box_start_col + 3; c++) {
            if (board->cells_row_major[r * 9 + c] == value) {
                return 0; 
            }
        }
    }
    
    return 1;
}

static int solve_recursive_cache_friendly(struct Board_CacheOptimized* board, int row, int col) {
    
    if (row == 9) {
        return 1;
    } 
    else if (col == 9) {
        return solve_recursive_cache_friendly(board, row + 1, 0);
    } 
    if (board->cells_row_major[row * 9 + col] != 0) {
        return solve_recursive_cache_friendly(board, row, col + 1);
    } 
    
    else {
        for (uint8_t p = 1; p <= 9; p++) {
            
            if (!is_valid_cache_friendly(board, row, col, p)) {
                continue;
            }
            
            int row_major_idx = row * 9 + col;
            int col_major_idx = col * 9 + row;

            board->cells_row_major[row_major_idx] = p;
            board->cells_col_major[col_major_idx] = p;
            
            if (solve_recursive_cache_friendly(board, row, col + 1)) {
                return 1;
            }
            
            board->cells_row_major[row_major_idx] = 0;
            board->cells_col_major[col_major_idx] = 0;
        }
        return 0;
    }
}

int solve_optimized_v3( struct Board_CacheOptimized* input, struct Board_CacheOptimized* solution) {

    memcpy(solution, input, sizeof(struct Board_CacheOptimized));

    return solve_recursive_cache_friendly(solution, 0, 0);
}



int read_file2(struct Board_CacheOptimized* board, const char* filename) {

    FILE* file = fopen(filename, "r");
    if (!file)
        return 1;

    int cell_index = 0;
    int c;

    while (cell_index < 81 && (c = fgetc(file)) != EOF) {
        if (c >= '0' && c <= '9') {

            uint8_t value = (uint8_t)(c - '0');

            board->cells_row_major[cell_index] = value;

            int row = cell_index / 9;
            int col = cell_index % 9;
         
            board->cells_col_major[col * 9 + row] = value;

            cell_index++;

        } else if (c != '\n' && c != '\r') {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    
    return (cell_index == 81) ? 0 : 1;
}


int is_board_valid_cache_optimized(const struct Board_CacheOptimized* board) {

    // seen_rows[r][num] == 1 means that the number 'num' has been seen on the row 'r'
    uint8_t seen_rows[9][10] = {0};
    uint8_t seen_cols[9][10] = {0};
    uint8_t seen_boxes[9][10] = {0};

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            
            uint8_t value = board->cells_row_major[r * 9 + c];

            // Ignore the empty cells
            if (value == 0) {
                continue;
            }

            // Check the row
            if (seen_rows[r][value]) {
                return 0; // Invalid: A duplicate found in this row
            }
            seen_rows[r][value] = 1; // Mark as "seen"

            // Check the column
            if (seen_cols[c][value]) {
                return 0; // Invalid: A duplicate found in this column
            }
            seen_cols[c][value] = 1; // Mark as "seen"

            // Verificăm cutia 3x3
            int box_idx = (r / 3) * 3 + (c / 3);
            if (seen_boxes[box_idx][value]) {
                return 0; // Invalid: Duplicate in this grid
            }
            seen_boxes[box_idx][value] = 1; // Mark as "seen"
        }
    }

    return 1;
}


void print_board2(const struct Board_CacheOptimized* board)
{
    const uint8_t* cells = board->cells_row_major;

    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {            
            putchar('0' + cells[row * 9 + col]);
        }

        putchar('\n');
    }
}