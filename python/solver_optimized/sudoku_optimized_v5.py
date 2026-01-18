#!/usr/bin/env python3
import heapq
import sys
import os
import time
from collections import deque
from typing import List, Optional, Tuple, Set

Pos = Tuple[int, int]
# precompute bidx using BOX_MAP
BOX_MAP = [
    [0, 0, 0, 1, 1, 1, 2, 2, 2],
    [0, 0, 0, 1, 1, 1, 2, 2, 2],
    [0, 0, 0, 1, 1, 1, 2, 2, 2],
    [3, 3, 3, 4, 4, 4, 5, 5, 5],
    [3, 3, 3, 4, 4, 4, 5, 5, 5],
    [3, 3, 3, 4, 4, 4, 5, 5, 5],
    [6, 6, 6, 7, 7, 7, 8, 8, 8],
    [6, 6, 6, 7, 7, 7, 8, 8, 8],
    [6, 6, 6, 7, 7, 7, 8, 8, 8],
]
ALL_NUMS = (1 << 9) - 1


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


def build_masks(board: List[List[int]]):
    """
    Builds digit masks for numbers in rows, columns, and boxes.
    """
    row_digits = [0] * 9
    col_digits = [0] * 9
    box_digits = [0] * 9

    for r in range(9):
        for c in range(9):
            val = board[r][c]
            if val != 0:
                mask = 1 << (board[r][c] - 1)
                box_digits[BOX_MAP[r][c]] |= mask
                row_digits[r] |= mask
                col_digits[c] |= mask
    return row_digits, col_digits, box_digits


def naked_singles_pre_processing(board, row_mask, col_mask, box_mask):
    """
    Method with naked-single propagation using a queue and bit masking
    Inspired by: https://github.com/knottwill/SudokuMaster/blob/main/src/engine/elimination.py
    """
    q = deque()
    in_queue = [[False] * 9 for _ in range(9)]
    # initial fill of queue with all empty cells
    for r in range(9):
        for c in range(9):
            if board[r][c] == 0:
                q.append((r, c))
                in_queue[r][c] = True
    while q:
        r, c = q.popleft()
        in_queue[r][c] = False
        if board[r][c] != 0:
            continue
        bidx = BOX_MAP[r][c]
        used_mask = row_mask[r] | col_mask[c] | box_mask[bidx]
        candidates = ALL_NUMS & ~used_mask
        cnt = candidates.bit_count()
        if cnt == 0:
            # contradiction, so there are no candidates
            return False
        if cnt == 1:
            digit = candidates & -candidates
            num = int(digit).bit_length()
            board[r][c] = num
            row_mask[r] |= digit
            col_mask[c] |= digit
            box_mask[bidx] |= digit
            # row
            for nc in range(9):
                if board[r][nc] == 0 and not in_queue[r][nc]:
                    q.append((r, nc))
                    in_queue[r][nc] = True
            # column
            for nr in range(9):
                if board[nr][c] == 0 and not in_queue[nr][c]:
                    q.append((nr, c))
                    in_queue[nr][c] = True
            # box
            br = (r // 3) * 3
            bc = (c // 3) * 3
            for rr in range(br, br + 3):
                for cc in range(bc, bc + 3):
                    if board[rr][cc] == 0 and not in_queue[rr][cc]:
                        q.append((rr, cc))
                        in_queue[rr][cc] = True
    return True

def find_mrv(board, row_mask, col_mask, box_mask):
    """
    Finds the MRV values by using a heap and bit masking
    :param board:
    :param row_mask:
    :param col_mask:
    :param box_mask:
    :return:
    """
    heap = list()
    for row in range(9):
        for col in range(9):
            if board[row][col] == 0:
                bidx = BOX_MAP[row][col]
                used_mask = (row_mask[row] | col_mask[col] | box_mask[bidx])
                candidates = ALL_NUMS & ~used_mask
                cand_cnt = candidates.bit_count()
                if cand_cnt == 0:
                    return None, 0
                # if candidate count is 1, best solution found
                if cand_cnt == 1:
                    return (row, col), candidates
                # Store candidate length, row, col and the candidates in the heap
                heapq.heappush(heap, (cand_cnt, row, col, candidates))
    if not heap:
        return None, None
    # Get row, col and candidates from the heap by poping these values
    _, row, col, candidates = heapq.heappop(heap)
    return (row, col), candidates


def solve_puzzle(board: List[List[int]], row_mask, col_mask, box_mask) -> bool:
    """
    The recursive method to solve the puzzle.
    :return:
    """
    best_pos, candidates_mask = find_mrv(board, row_mask, col_mask, box_mask)
    if best_pos is None:
        if candidates_mask is None:
            return True
        else:
            return False
    row, col = best_pos
    bidx = BOX_MAP[row][col]

    while candidates_mask:
        digit = candidates_mask & -candidates_mask
        num = digit.bit_length()
        board[row][col] = num
        row_mask[row] |= digit
        col_mask[col] |= digit
        box_mask[bidx] |= digit

        if solve_puzzle(board, row_mask, col_mask, box_mask):
            return True
        board[row][col] = 0
        # Removing digit using NOT operator
        row_mask[row] ^= digit
        col_mask[col] ^= digit
        box_mask[bidx] ^= digit
        # Removes digit using NOT operator from candidates mask
        candidates_mask ^= digit

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
        row_used, col_used, box_used = build_masks(board)
    except Exception as e:
        print(f"Error reading data: {e}")
        return 1

    print("Puzzle to solve:")
    print_board(board)

    print('\nSolution found:')
    start = time.time()
    if not naked_singles_pre_processing(board, row_used, col_used, box_used):
        print("Contradiction found processing naked singles")
        return 1
    solve_puzzle(board, row_used, col_used, box_used)
    print_board(board)
    end = time.time()
    print(f"Runtime: {end - start:.6f} seconds")


if __name__ == '__main__':
    raise SystemExit(main(sys.argv))
