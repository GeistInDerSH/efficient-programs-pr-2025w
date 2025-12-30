#!/usr/bin/env python3
import sys
import os
from typing import List, Optional, Tuple


class Sudoku_Solver_Optimized_V1:
    def __init__(self, board):
        self.board = board

    def read_board(self, path: str):
        """
        Reads the test files from the dataset directory
        :param path:
        """
        current_directory = os.path.abspath('')
        cleaned_path = current_directory.replace("\\solver_unoptimized", "")
        relative_path_dataset = "\\dataset\\" + path
        full_path = cleaned_path + relative_path_dataset
        with open(full_path, "r", encoding="utf-8") as f:
            lines = [ln.strip() for ln in f.readlines() if ln.strip()]
        for i, ln in enumerate(lines):
            row = []
            for ch in ln:
                if ch == "." or ch == "0":
                    row.append(0)
                elif ch.isdigit() and 1 <= int(ch) <= 9:
                    row.append(int(ch))
            self.board.append(row)

    def cacheValues(self):
        cache = dict()
        for i in range(9):
            for j in range(9):
                if self.board[i][j] == 0:
                    cache[(i, j)] = self.allowedValues(i, j)
        return cache

    def allowedValues(self, row, col):
        # collect number in row
        used = set(x for x in self.board[row] if x != 0)
        # collect number in column
        used.update(self.board[r][col] for r in range(9) if self.board[r][col] != 0)
        # collect numbers from the 3x3 block
        block_row = (row // 3) * 3
        block_col = (col // 3) * 3
        for rw in range(block_row, block_row + 3):
            for cl in range(block_col, block_col + 3):
                value = self.board[rw][cl]
                if value != 0:
                    used.add(value)
        return [num for num in range(1, 10) if num not in used]

    def print_board(self):
        """
        Prints the current puzzle values
        """
        for i in range(9):
            for j in range(9):
                print(self.board[i][j], end=" "),
            print()

    def is_valid(self, row, col, number):
        """
        Checks, if a row, column or a block is valid.
        It's valid, if in a row, column or a block have no numbers
        with multiple occurrences
        :param row:
        :param col:
        :param number:
        :return:
        """
        if any(self.board[row][c] == number for c in range(9)):
            return False
        if any(self.board[r][col] == number for r in range(9)):
            return False
        block_row = (row // 3) * 3
        block_col = (col // 3) * 3
        for rw in range(block_row, block_row + 3):
            for cl in range(block_col, block_col + 3):
                if self.board[rw][cl] == number:
                    return False
        return True

    def solve_puzzle_with_cache(self):
        """
        The recursive method to solve the puzzle.
        Optimized version uses dictionary to cache numbers
        :return:
        """
        cache = self.cacheValues()
        if not cache:
            return True
        pos = min(cache.keys(), key=lambda k: len(cache[k]))
        row, col = pos
        candidate_numbers = list(cache[pos])
        for num in candidate_numbers:
            if self.is_valid(row, col, num):
                self.board[row][col] = num
                if self.solve_puzzle_with_cache():
                    return True
                self.board[row][col] = 0
        return False


def main(argv):
    """
    The main method which reads the argument values.
    If no argument values are used, the default ex.txt file is used
    :param argv:
    :return:
    """
    if len(argv) > 1:
        path = argv[1]
    else:
        path = "ex.txt"
    sudoku_solver = Sudoku_Solver_Optimized_V1([])
    try:
        sudoku_solver.read_board(path)
    except Exception as e:
        print(f"Error reading data: {e}")
        return 1

    print("Puzzle to solve:")
    sudoku_solver.print_board()

    if sudoku_solver.solve_puzzle_with_cache():
        print('\nSolution found:')
        sudoku_solver.print_board()


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
