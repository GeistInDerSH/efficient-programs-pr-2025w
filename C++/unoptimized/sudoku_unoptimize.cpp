#include "sudoku_unoptimize.hpp"
#include "../common/board.hpp"

#include <fstream>
#include <iostream>

// Função auxiliar propositadamente má (impede otimizações)
static std::uint8_t get_cell(const Board& board, int index) {
    return board.cells[index];
}

// Função auxiliar: verifica se 'value' é válido em (row, col)
// Versão intencionalmente NÃO otimizada
static int is_valid(const Board& board, int row, int col, std::uint8_t value) {

    // único na linha
    for (int v = 0; v < 9; ++v) {
        int idx = row * 9 + v;
        if (value != 0) {
            if (value == get_cell(board, idx)) {
                return 0;
            }
        }
    }

    // único na coluna
    for (int v = 0; v < 9; ++v) {
        int idx = v * 9 + col;
        if (value != 0) {
            if (value == get_cell(board, idx)) {
                return 0;
            }
        }
    }

    // único na caixa 3x3
    int box_start_col = (col / 3) * 3;
    int box_end_col   = box_start_col + 3;
    int box_start_row = (row / 3) * 3;
    int box_end_row   = box_start_row + 3;

    for (int r = box_start_row; r < box_end_row; ++r) {
        for (int c = box_start_col; c < box_end_col; ++c) {
            int idx = r * 9 + c;
            if (value != 0) {
                if (value == get_cell(board, idx)) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

// Backtracking recursivo (versão mais pesada)
static int solve_recursive(Board& board, int row, int col) {

    if (row == 9) {
        return 1;
    } else {
        if (col == 9) {
            return solve_recursive(board, row + 1, 0);
        } else {
            int idx = row * 9 + col;
            if (get_cell(board, idx) != 0) {
                return solve_recursive(board, row, col + 1);
            } else {

                for (std::uint8_t p = 1; p <= 9; ++p) {
                    if (is_valid(board, row, col, p)) {
                        board.cells[row * 9 + col] = p;

                        if (solve_recursive(board, row, col + 1)) {
                            return 1;
                        }

                        board.cells[row * 9 + col] = 0;
                    }
                }
                return 0;
            }
        }
    }
}

// --- API pública ---

int read_file(Board& board, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 1;
    }

    int cell_index = 0;
    char c;

    while (cell_index < 81 && file.get(c)) {
        if (c >= '0' && c <= '9') {
            board.cells[cell_index] = static_cast<std::uint8_t>(c - '0');
            cell_index++;
        } else {
            if (c == '\n' || c == '\r') {
                // ignora
            } else {
                return 1;
            }
        }
    }

    if (cell_index == 81) {
        return 0;
    }
    return 1;
}

int solve(const Board& input, Board& solution) {
    solution = input;
    return solve_recursive(solution, 0, 0);
}

void print_board(const Board& board) {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            std::cout << static_cast<int>(
                get_cell(board, row * 9 + col)
            );
        }
        std::cout << '\n';
    }
}
