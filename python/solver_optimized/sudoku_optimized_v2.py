#!/usr/bin/env python3
import heapq
import sys
import os
import time
from typing import List, Optional, Tuple, Set

Pos = Tuple[int, int]


def read_board(path: str, board: List[List[int]]):
    """
    Reads the test files from the dataset directory
    :param path:
    """
    root_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))
    boards_dir = os.path.join(root_dir, "boards")
    full_path = os.path.join(boards_dir, path)
    with open(full_path, "r", encoding="utf-8") as f:
        lines = [ln.strip() for ln in f.readlines() if ln.strip()]
    for i, ln in enumerate(lines):
        row = []
        for ch in ln:
            if ch == "." or ch == "0":
                row.append(0)
            elif ch.isdigit() and 1 <= int(ch) <= 9:
                row.append(int(ch))
        board.append(row)


def print_board(board):
    """
    Prints the current puzzle values
    """
    for i in range(9):
        for j in range(9):
            print(board[i][j], end=" "),
        print()


def build_used_sets(board: List[List[int]]):
    """
    Builds sets for used numbers in rows, columns, and boxes.
    """
    row_used: List[Set[int]] = [set() for _ in range(9)]
    col_used: List[Set[int]] = [set() for _ in range(9)]
    box_used: List[Set[int]] = [set() for _ in range(9)]

    for r in range(9):
        for c in range(9):
            val = board[r][c]
            if val != 0:
                row_used[r].add(val)
                col_used[c].add(val)
                bidx = (r // 3) * 3 + (c // 3)
                box_used[bidx].add(val)
    return row_used, col_used, box_used


def find_mrv(board, row_used, col_used, box_used):
    """
    Finds the MRV values by using a heap
    :param board:
    :param row_used:
    :param col_used:
    :param box_used:
    :return:
    """
    heap = list()
    for row in range(9):
        for col in range(9):
            if board[row][col] == 0:
                candidates = allowedValues(row, col, row_used, col_used, box_used)
                cand_len = len(candidates)
                if cand_len == 0:
                    return None, set()
                # if candidate count is 1, best solution found
                if cand_len == 1:
                    return (row, col), candidates
                # Store candidate length, row, col and the candidates in the heap
                heapq.heappush(heap, (len(candidates), row, col, candidates))
    if not heap:
        return None, None
    # Get row, col and candidates from the heap by poping these values
    _, row, col, candidates = heapq.heappop(heap)
    return (row, col), candidates


def allowedValues(row: int, col: int, row_used: List[Set[int]], col_used: List[Set[int]],
                  box_used: List[Set[int]]) -> Set[int]:
    """
    Returns the set of possible numbers
    using the precomputed row, col, box used sets.
    """
    bidx = (row // 3) * 3 + (col // 3)
    return set(range(1, 10)) - (row_used[row] | col_used[col] | box_used[bidx])


def solve_puzzle(board: List[List[int]],
                 row_used: List[Set[int]],
                 col_used: List[Set[int]],
                 box_used: List[Set[int]]) -> bool:
    """
    The recursive method to solve the puzzle.
    :return:
    """
    best_pos, candidates = find_mrv(board, row_used, col_used, box_used)
    if best_pos is None:
        if candidates is None:
            return True
        else:
            return False
    row, col = best_pos
    bidx = (row // 3) * 3 + (col // 3)

    for num in candidates:
        board[row][col] = num
        row_used[row].add(num)
        col_used[col].add(num)
        box_used[bidx].add(num)

        if solve_puzzle(board, row_used, col_used, box_used):
            return True
        board[row][col] = 0
        row_used[row].remove(num)
        col_used[col].remove(num)
        box_used[bidx].remove(num)

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
        path = "fully-solved.sudoku"
    try:
        board: List[List[int]] = []
        read_board(path, board)
        row_used, col_used, box_used = build_used_sets(board)
    except Exception as e:
        print(f"Error reading data: {e}")
        return 1

    print("Puzzle to solve:")
    print_board(board)

    print('\nSolution found:')
    start = time.time()
    solve_puzzle(board, row_used, col_used, box_used)
    print_board(board)
    end = time.time()
    print(f"Runtime: {end - start:.6f} seconds")


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
