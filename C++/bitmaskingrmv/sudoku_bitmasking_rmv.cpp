#include "sudoku_bitmasking_rmv.hpp"
#include "../common/board.hpp"

#include <fstream>
#include <iostream>

static constexpr uint16_t FULL_MASK = 0x1FF; // 9 bits ligados (111111111)

// Bitmasks globais de estado
static uint16_t row_mask[9];
static uint16_t col_mask[9];
static uint16_t box_mask[9];

// --------------------------------------------------

static inline int box_index(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

static inline int count_bits(uint16_t x) {
    return __builtin_popcount(x);
}

// --------------------------------------------------
// MRV: escolhe a célula vazia com menos opções

static bool find_best_cell(const Board& board,
                           int& best_r,
                           int& best_c,
                           uint16_t& best_mask,
                           bool& has_empty) {
    int min_count = 10;
    has_empty = false;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            if (board.cells[idx] == 0) {
                has_empty = true;

                int b = box_index(r, c);
                uint16_t used = row_mask[r] | col_mask[c] | box_mask[b];
                uint16_t avail = (~used) & FULL_MASK;
                int cnt = count_bits(avail);

                if (cnt == 0)
                    return false; // contradição

                if (cnt < min_count) {
                    min_count = cnt;
                    best_r = r;
                    best_c = c;
                    best_mask = avail;

                    if (cnt == 1)
                        return true;
                }
            }
        }
    }
    return has_empty;
}

// --------------------------------------------------

static bool solve_recursive(Board& board) {
    int r, c;
    uint16_t avail_mask;
    bool has_empty;

    bool ok = find_best_cell(board, r, c, avail_mask, has_empty);

    if (!has_empty)
        return true; // resolvido

    if (!ok)
        return false; // contradição

    int b = box_index(r, c);
    int idx = r * 9 + c;

    while (avail_mask) {
        uint16_t bit = avail_mask & -avail_mask;
        avail_mask -= bit;

        int value = __builtin_ctz(bit) + 1;

        board.cells[idx] = value;
        row_mask[r] |= bit;
        col_mask[c] |= bit;
        box_mask[b] |= bit;

        if (solve_recursive(board))
            return true;

        board.cells[idx] = 0;
        row_mask[r] ^= bit;
        col_mask[c] ^= bit;
        box_mask[b] ^= bit;
    }

    return false;
}

// --------------------------------------------------
// API pública

int read_file(Board& board, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 1;
    }

    int cell_index = 0;
    char c;

    while (cell_index < 81 && file.get(c)) {
        if (c >= '0' && c <= '9') {
            board.cells[cell_index++] = static_cast<uint8_t>(c - '0');
        }
    }

    return cell_index == 81 ? 0 : 1;
}

int solve(const Board& input, Board& solution) {
    solution = input;

    for (int i = 0; i < 9; i++) {
        row_mask[i] = col_mask[i] = box_mask[i] = 0;
    }

    // Inicializa máscaras
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            int val = solution.cells[idx];

            if (val != 0) {
                uint16_t bit = 1 << (val - 1);
                int b = box_index(r, c);

                row_mask[r] |= bit;
                col_mask[c] |= bit;
                box_mask[b] |= bit;
            }
        }
    }

    return solve_recursive(solution) ? 1 : 0;
}

void print_board(const Board& board) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            std::cout << int(board.cells[r * 9 + c]);
        }
        std::cout << "\n";
    }
}
