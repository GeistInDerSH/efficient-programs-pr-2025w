# Hybrid Logic + MRV + Forward Checking + LCV

## What Changed Compared to Bitmasking + MRV + Forward Checking

This version adds a **logic-based constraint layer and value-ordering heuristics** on top of the existing bitmasking, MRV, and forward checking solver.

---

## 1. Deterministic Logic Propagation

### Code Change
Before backtracking, the solver applies human-style Sudoku logic:

- Naked Singles  
  Cells with only one valid value are filled immediately.

- Hidden Singles (row-based)  
  If a value can only appear in one position in a row, it is placed.

This is implemented in:
apply_logic(...)

### Efficiency Impact
- Reduces the number of empty cells before search starts
- Shrinks the depth of the recursion tree
- Eliminates trivial branches without any backtracking
- Improves performance on easy and medium puzzles

---

## 2. Least Constraining Value (LCV)

### Code Change
When multiple values are possible for a chosen cell, values are ordered so that the one that restricts neighboring cells the least is tried first.

This is done by scoring how much a value would reduce available options in the row and column, then sorting candidates.

### Efficiency Impact
- Increases the chance of finding a valid solution early
- Reduces the number of failed recursive paths
- Makes backtracking more efficient on ambiguous puzzles

---

## 3. Combined Heuristic Pipeline

### Code Change
The solver now follows a multi-stage strategy:
1. Apply logic-based propagation
2. Select cell using MRV
3. Order values using LCV
4. Use forward checking and backtracking

### Efficiency Impact
- Combines structural pruning with heuristic-driven search
- Produces stable runtime across difficulty levels
- Minimizes unnecessary recursion compared to FC-only solvers

---

## Overall Performance Effect

Compared to Bitmasking + MRV + FC:
- Fewer recursive calls
- Better value ordering
- Faster convergence on easy and medium puzzles
- More consistent performance across all difficulties

---

## Summary

This solver represents a **hybrid between human-style logic and algorithmic search**.

Instead of relying purely on constraint propagation or purely on backtracking, it blends:
- Deterministic deductions
- Smart variable selection (MRV)
- Smart value ordering (LCV)
- Efficient state tracking (bitmasking + FC)

This makes it the most balanced and robust solver in the project.
