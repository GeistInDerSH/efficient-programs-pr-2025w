# Bitmasking + Minimum Remaining Values (MRV)

## What Changed Compared to Unoptimized

This version introduces two major optimizations over the baseline solver:

---

## 1. Bitmask-Based Constraint Tracking

### Code Change
Instead of scanning rows, columns, and 3×3 boxes every time a value is tested, this solver maintains three global bitmask arrays:

- row_mask[9]
- col_mask[9]
- box_mask[9]

Each bit represents whether a digit (1–9) is already used in that row, column, or box.

Valid values for a cell are computed using:
used = row_mask[r] | col_mask[c] | box_mask[b]  
available = (~used) & FULL_MASK

### Efficiency Impact
- Replaces 27 memory checks per value attempt with a few bitwise operations
- Eliminates repeated scanning of the board
- Greatly reduces CPU instructions per recursive step
- Improves cache locality and branch predictability

---

## 2. Minimum Remaining Values (MRV) Heuristic

### Code Change
Before choosing a cell to fill, the solver scans the board and selects the empty cell with the fewest valid candidates:

find_best_cell(...)

This means the solver always works on the most constrained cell first.

### Efficiency Impact
- Dramatically reduces the size of the search tree
- Detects contradictions earlier
- Avoids exploring large invalid branches
- Especially effective on hard and extra-hard puzzles

---

## Overall Performance Effect

Compared to the unoptimized solver, this version:
- Explores far fewer recursive paths
- Performs fewer memory accesses
- Uses mostly integer and bitwise operations
- Scales much better with puzzle difficulty

This results in large reductions in both wall-clock time and CPU instructions, especially on medium and hard Sudoku boards.

---

## Summary

The transition from full board scanning to bitmasking changes the solver from a memory-heavy, branch-heavy algorithm into a compute-efficient, heuristic-driven search.

This version serves as the foundation for further optimizations such as Forward Checking and Hybrid heuristics.
