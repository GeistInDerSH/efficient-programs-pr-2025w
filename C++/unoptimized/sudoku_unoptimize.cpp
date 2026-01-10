#include "sudoku_unoptimize.hpp"
#include <fstream>
#include <iostream>

// Função auxiliar: verifica se 'value' é válido em (row, col)
static int is_valid(const Board& board, int row, int col, std::uint8_t value) {
    // único na linha
    for (int v = 0; v < 9; ++v) {
        if (value == board.cells[row * 9 + v]) {
            return 0;
        }
    }

    // único na coluna
    for (int v = 0; v < 9; ++v) {
        if (value == board.cells[v * 9 + col]) {
            return 0;
        }
    }

    // único na caixa 3x3
    int box_start_col = (col / 3) * 3;
    int box_end_col   = box_start_col + 3;
    int box_start_row = (row / 3) * 3;
    int box_end_row   = box_start_row + 3;

    for (int r = box_start_row; r < box_end_row; ++r) {
        for (int c = box_start_col; c < box_end_col; ++c) {
            if (value == board.cells[r * 9 + c]) {
                return 0;
            }
        }
    }

    return 1;
}

// Backtracking recursivo (igual ao original)
static int solve_recursive(Board& board, int row, int col) {
    if (row == 9) {
        return 1; // todas as linhas resolvidas
    } else if (col == 9) {
        return solve_recursive(board, row + 1, 0); // próxima linha
    } else if (board.cells[row * 9 + col] != 0) {
        return solve_recursive(board, row, col + 1); // célula já preenchida
    } else {
        // Tenta todos os números
        for (std::uint8_t p = 1; p <= 9; ++p) {
            if (!is_valid(board, row, col, p)) {
                continue;
            }
            board.cells[row * 9 + col] = p; // tentativa
            if (solve_recursive(board, row, col + 1)) {
                return 1; // resultou
            }
            board.cells[row * 9 + col] = 0; // backtrack
        }
        return 0; // nenhum número válido
    }
}

// --- API pública ---

int read_file(Board& board, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 1; // erro
    }

    int cell_index = 0;
    char c;

    while (cell_index < 81 && file.get(c)) {
        if (c >= '0' && c <= '9') {
            board.cells[cell_index] = static_cast<std::uint8_t>(c - '0');
            ++cell_index;
        } else if (c == '\n' || c == '\r') {
            // ignora quebras de linha
            continue;
        } else {
            // carácter inválido
            return 1;
        }
    }

    return (cell_index == 81) ? 0 : 1; // 0 se leu 81 dígitos
}

int solve(const Board& input, Board& solution) {
    // Cópia simples, equivalente a "*solution = *input" em C
    solution = input;
    return solve_recursive(solution, 0, 0);
}

void print_board(const Board& board) {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            std::cout << static_cast<int>(board.cells[row * 9 + col]);
        }
        std::cout << '\n';
    }
}
