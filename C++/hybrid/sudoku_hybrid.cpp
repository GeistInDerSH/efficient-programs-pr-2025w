#include "sudoku_hybrid.hpp"
#include "../common/board.hpp"

#include <fstream>
#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm>

// -------------------------------------
// Bitmask helpers

static constexpr uint16_t FULL_MASK = 0x1FF;

static inline int box_index(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

static inline int popcount(uint16_t x) {
    return __builtin_popcount(x);
}

static inline int lsb_index(uint16_t x) {
    return __builtin_ctz(x);
}

// -------------------------------------

static uint16_t row_mask[9];
static uint16_t col_mask[9];
static uint16_t box_mask[9];

// -------------------------------------
// Constraint Propagation

static bool apply_logic(Board& board) {
    bool progress = true;

    while (progress) {
        progress = false;

        // --- Naked Singles
        for (int r = 0; r < 9; r++) {
            for (int c = 0; c < 9; c++) {
                int idx = r * 9 + c;
                if (board.cells[idx] != 0)
                    continue;

                int b = box_index(r, c);
                uint16_t used = row_mask[r] | col_mask[c] | box_mask[b];
                uint16_t avail = (~used) & FULL_MASK;

                if (avail == 0)
                    return false;

                if (popcount(avail) == 1) {
                    int v = lsb_index(avail);
                    uint16_t bit = 1 << v;

                    board.cells[idx] = v + 1;
                    row_mask[r] |= bit;
                    col_mask[c] |= bit;
                    box_mask[b] |= bit;

                    progress = true;
                }
            }
        }

        // --- Hidden Singles (row-based)
        for (int r = 0; r < 9; r++) {
            for (int v = 0; v < 9; v++) {
                uint16_t bit = 1 << v;
                if (row_mask[r] & bit)
                    continue;

                int count = 0;
                int last_c = -1;

                for (int c = 0; c < 9; c++) {
                    int idx = r * 9 + c;
                    if (board.cells[idx] != 0)
                        continue;

                    int b = box_index(r, c);
                    uint16_t used = row_mask[r] | col_mask[c] | box_mask[b];
                    if (!(used & bit)) {
                        count++;
                        last_c = c;
                    }
                }

                if (count == 1) {
                    int idx = r * 9 + last_c;
                    board.cells[idx] = v + 1;

                    row_mask[r] |= bit;
                    col_mask[last_c] |= bit;
                    box_mask[box_index(r, last_c)] |= bit;

                    progress = true;
                }
            }
        }
    }

    return true;
}

// -------------------------------------
// MRV + LCV

static bool find_best_cell(const Board& board,
                           int& out_r,
                           int& out_c,
                           std::vector<int>& ordered_values) {
    int min_count = 10;
    bool found = false;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            if (board.cells[idx] != 0)
                continue;

            int b = box_index(r, c);
            uint16_t used = row_mask[r] | col_mask[c] | box_mask[b];
            uint16_t avail = (~used) & FULL_MASK;

            int cnt = popcount(avail);
            if (cnt == 0)
                return false;

            if (cnt < min_count) {
                min_count = cnt;
                out_r = r;
                out_c = c;
                found = true;

                ordered_values.clear();
                while (avail) {
                    uint16_t bit = avail & -avail;
                    avail -= bit;
                    ordered_values.push_back(lsb_index(bit));
                }

                if (cnt == 1)
                    return true;
            }
        }
    }

    // --- LCV: ordenar valores menos restritivos primeiro
    auto score = [&](int v) {
        int impact = 0;
        uint16_t bit = 1 << v;

        for (int i = 0; i < 9; i++) {
            // row
            if (!(row_mask[out_r] & bit))
                impact++;
            // col
            if (!(col_mask[out_c] & bit))
                impact++;
        }
        return impact;
    };

    std::sort(ordered_values.begin(), ordered_values.end(),
              [&](int a, int b) {
                  return score(a) < score(b);
              });

    return found;
}

// -------------------------------------
// Backtracking

static bool solve_recursive(Board& board) {
    if (!apply_logic(board))
        return false;

    int r, c;
    std::vector<int> values;

    if (!find_best_cell(board, r, c, values))
        return true;

    int idx = r * 9 + c;
    int b = box_index(r, c);

    for (int v : values) {
        uint16_t bit = 1 << v;

        board.cells[idx] = v + 1;
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

// -------------------------------------
// API

int solve(const Board& input, Board& solution) {
    solution = input;

    for (int i = 0; i < 9; i++) {
        row_mask[i] = col_mask[i] = box_mask[i] = 0;
    }

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            int v = solution.cells[idx];
            if (v != 0) {
                uint16_t bit = 1 << (v - 1);
                row_mask[r] |= bit;
                col_mask[c] |= bit;
                box_mask[box_index(r, c)] |= bit;
            }
        }
    }

    return solve_recursive(solution) ? 1 : 0;
}

// -------------------------------------
// IO

int read_file(Board& board, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        return 1;

    int idx = 0;
    char c;
    while (idx < 81 && file.get(c)) {
        if (c >= '0' && c <= '9')
            board.cells[idx++] = static_cast<uint8_t>(c - '0');
    }

    return idx == 81 ? 0 : 1;
}

void print_board(const Board& board) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++)
            std::cout << int(board.cells[r * 9 + c]);
        std::cout << "\n";
    }
}
