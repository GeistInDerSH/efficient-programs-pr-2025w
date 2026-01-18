#include "sudoku_bitmasking_rmv_fc.hpp"
#include "../common/board.hpp"

#include <cstdint>
#include <vector>

#include <fstream>
#include <string>
#include <iostream>

static constexpr uint16_t FULL_MASK = 0x1FF; // 9 bits

static uint16_t row_mask[9];
static uint16_t col_mask[9];
static uint16_t box_mask[9];

// Domínios das células (forward checking)
static uint16_t domain[81];

static inline int box_index(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

static inline int count_bits(uint16_t x) {
    return __builtin_popcount(x);
}

// --------------------------------------------------
// Inicializa domínios a partir das máscaras
static void init_domains(const Board& board) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            if (board.cells[idx] != 0) {
                domain[idx] = 0;
            } else {
                int b = box_index(r, c);
                uint16_t used = row_mask[r] | col_mask[c] | box_mask[b];
                domain[idx] = (~used) & FULL_MASK;
            }
        }
    }
}

// --------------------------------------------------
// MRV com domínios
static bool find_best_cell(int& best_idx) {
    int min_count = 10;
    best_idx = -1;

    for (int i = 0; i < 81; i++) {
        if (domain[i] != 0) {
            int cnt = count_bits(domain[i]);
            if (cnt == 0)
                return false; // contradição

            if (cnt < min_count) {
                min_count = cnt;
                best_idx = i;
                if (cnt == 1)
                    return true;
            }
        }
    }
    return best_idx != -1;
}

// --------------------------------------------------
// Forward checking: remove valor dos vizinhos

struct Change {
    int idx;
    uint16_t old_domain;
};

static bool propagate(int idx, uint16_t bit, std::vector<Change>& changes) {
    int r = idx / 9;
    int c = idx % 9;
    int b = box_index(r, c);

    for (int i = 0; i < 9; i++) {
        int row_i = r * 9 + i;
        int col_i = i * 9 + c;

        if (domain[row_i] & bit) {
            changes.push_back({row_i, domain[row_i]});
            domain[row_i] &= ~bit;
            if (domain[row_i] == 0 && row_i != idx)
                return false;
        }

        if (domain[col_i] & bit) {
            changes.push_back({col_i, domain[col_i]});
            domain[col_i] &= ~bit;
            if (domain[col_i] == 0 && col_i != idx)
                return false;
        }
    }

    int br = (b / 3) * 3;
    int bc = (b % 3) * 3;

    for (int dr = 0; dr < 3; dr++) {
        for (int dc = 0; dc < 3; dc++) {
            int bi = (br + dr) * 9 + (bc + dc);
            if (domain[bi] & bit) {
                changes.push_back({bi, domain[bi]});
                domain[bi] &= ~bit;
                if (domain[bi] == 0 && bi != idx)
                    return false;
            }
        }
    }

    return true;
}

static void undo(const std::vector<Change>& changes) {
    for (auto it = changes.rbegin(); it != changes.rend(); ++it) {
        domain[it->idx] = it->old_domain;
    }
}

// --------------------------------------------------

static bool solve_recursive(Board& board) {
    int idx;
    if (!find_best_cell(idx))
        return true; // resolvido

    uint16_t avail = domain[idx];
    int r = idx / 9;
    int c = idx % 9;
    int b = box_index(r, c);

    while (avail) {
        uint16_t bit = avail & -avail;
        avail -= bit;

        int value = __builtin_ctz(bit) + 1;

        board.cells[idx] = value;
        row_mask[r] |= bit;
        col_mask[c] |= bit;
        box_mask[b] |= bit;

        std::vector<Change> changes;
        uint16_t old_domain = domain[idx];
        domain[idx] = 0;

        bool ok = propagate(idx, bit, changes);

        if (ok && solve_recursive(board))
            return true;

        // undo
        board.cells[idx] = 0;
        row_mask[r] ^= bit;
        col_mask[c] ^= bit;
        box_mask[b] ^= bit;

        domain[idx] = old_domain;
        undo(changes);
    }

    return false;
}

// --------------------------------------------------

int solve(const Board& input, Board& solution) {
    solution = input;

    for (int i = 0; i < 9; i++)
        row_mask[i] = col_mask[i] = box_mask[i] = 0;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            int v = solution.cells[idx];
            if (v != 0) {
                uint16_t bit = 1 << (v - 1);
                int b = box_index(r, c);
                row_mask[r] |= bit;
                col_mask[c] |= bit;
                box_mask[b] |= bit;
            }
        }
    }

    init_domains(solution);

    return solve_recursive(solution) ? 1 : 0;
}

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

void print_board(const Board& board) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            std::cout << int(board.cells[r * 9 + c]);
        }
        std::cout << "\n";
    }
}
