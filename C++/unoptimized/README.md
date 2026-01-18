# Unoptimized Sudoku Solver

## Overview

This solver implements a naive recursive backtracking algorithm for solving 9×9 Sudoku puzzles.
It is intentionally written without performance optimizations and serves as a baseline reference
for comparing more advanced techniques such as bitmasking, heuristic-based search, and exact cover (DLX).

The goal of this version is not efficiency, but clarity and contrast — making it easy to measure
and explain the impact of algorithmic and implementation-level optimizations.

---

## Algorithm

The solver follows a simple depth-first search approach:

1. Traverse the board from top-left to bottom-right.
2. When an empty cell is found:
   - Try all values from 1 to 9
   - For each value, check if it is valid in:
     - The current row
     - The current column
     - The current 3×3 sub-grid
3. If valid, place the number and recurse to the next cell.
4. If a dead end is reached, backtrack by clearing the cell and trying the next value.

This process continues until the board is fully filled or all possibilities are exhausted.

---

## Why This Version Is Unoptimized

This implementation deliberately avoids common performance improvements.

### 1. Full Constraint Re-Checking

For every attempted value, the solver:
- Scans the entire row (9 checks)
- Scans the entire column (9 checks)
- Scans the entire 3×3 box (9 checks)

This results in:
27 memory accesses per candidate value, per recursive step.

No intermediate state or precomputed constraints are stored.

---

### 2. No Heuristics

The solver always fills cells in a fixed order:
- Left to right
- Top to bottom

It does not:
- Choose the cell with the fewest valid candidates (MRV)
- Try more promising values first (LCV)
- Detect contradictions early (Forward Checking)

This causes:
Large and unnecessary search trees, especially on hard puzzles.

---

### 3. No State Caching

The solver does not maintain:
- Row usage tables
- Column usage tables
- Box usage tables
- Bitmasks or lookup arrays

All validity checks are computed from scratch every time.

---

### 4. Extra Function Call Overhead

Access to a board cell is done through a helper function instead of direct array access.

This:
- Prevents aggressive compiler inlining
- Adds unnecessary function call overhead in tight loops

This is intentional to further amplify performance differences.

---

## Time Complexity

This solver operates in exponential time in the worst case:

O(9^n), where n is the number of empty cells.

Since it performs no pruning beyond basic constraint checks, it explores a very large portion
of the search space for difficult puzzles.

---

## Strengths

- Simple and easy to understand
- Correct and complete
- Ideal as a baseline for performance comparison
- Demonstrates the cost of naive backtracking clearly

---

## Weaknesses

- Very slow on medium, hard, and extreme puzzles
- Performs redundant work at every recursion level
- Does not scale well with puzzle difficulty
- High number of CPU cycles and instructions per solution

---

## Purpose in This Project

This solver is used as a reference implementation to evaluate the impact of:

- Bitmasking
- Minimum Remaining Values (MRV)
- Forward Checking
- Dancing Links (Algorithm X)
- Hybrid heuristic strategies

All optimized solvers in this project are compared against this version to quantify
performance improvements.

---
