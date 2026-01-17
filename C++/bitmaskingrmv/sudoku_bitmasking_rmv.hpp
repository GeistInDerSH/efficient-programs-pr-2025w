#pragma once

#include <array>
#include <cstdint>
#include <string>
#include "common/board.hpp"

/*
 * Lê um tabuleiro de um ficheiro.
 * Preenche "board".
 * Retorna 0 em sucesso, 1 em erro (tal como a versão em C).
 */
int read_file(Board& board, const std::string& filename);

/*
 * Resolve o Sudoku.
 * "input" é o puzzle original (não é modificado).
 * "solution" fica com a solução.
 * Retorna 1 se encontrou solução, 0 se não existe solução.
 */
int solve(const Board& input, Board& solution);

/*
 * Imprime o tabuleiro como 9 linhas de 9 dígitos.
 */
void print_board(const Board& board);
