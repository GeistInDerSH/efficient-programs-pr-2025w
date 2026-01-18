#pragma once

#include <string>
#include "../common/board.hpp"

int read_file(Board& board, const std::string& filename);
int solve(const Board& input, Board& solution);
void print_board(const Board& board);
