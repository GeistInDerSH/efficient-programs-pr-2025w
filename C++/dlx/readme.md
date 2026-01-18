# Dancing Links + Algorithm X (DLX)

## What Changed Compared to Bitmasking + MRV + Forward Checking

This version replaces recursive constraint solving entirely with an **Exact Cover formulation** using **Algorithm X** and the **Dancing Links (DLX)** data structure.

---

## 1. Problem Reformulation (Exact Cover Model)

### Code Change
The Sudoku problem is converted into a matrix with 324 constraint columns:

Each possible (row, column, value) assignment enforces:
- One value per cell
- One value per row
- One value per column
- One value per 3×3 box

Each candidate placement becomes a row in the matrix that satisfies exactly four constraints.

### Efficiency Impact
- Turns Sudoku into a pure constraint satisfaction problem
- Eliminates traditional backtracking logic
- Makes constraint handling uniform and mathematical instead of procedural

---

## 2. Dancing Links Data Structure

### Code Change
The solver uses a custom linked structure:
- Nodes are linked horizontally and vertically
- Columns track how many valid rows remain
- Constraints are removed and restored using pointer operations

### Efficiency Impact
- Cover/uncover operations run in constant time
- No memory allocation during search
- Extremely fast backtracking and undo
- Very cache-friendly compared to recursive domain updates

---

## 3. Built-in Heuristic (Minimum Column Size)

### Code Change
The algorithm always selects the column with the fewest remaining rows:
choose_column()

### Efficiency Impact
- Equivalent to MRV at the constraint level
- Minimizes branching factor automatically
- Strong pruning without explicit domain tracking

---

## Overall Performance Effect

Compared to heuristic solvers:
- Removes value ordering, forward checking, and domain management logic
- Uses pure structural pruning instead of repeated constraint propagation
- Has predictable performance across difficulty levels
- Excels on very constrained or highly ambiguous puzzles

---

## Summary

This solver shifts from a "search with heuristics" model to a **formal exact cover solver**.

Instead of guessing and checking, it:
- Systematically eliminates impossible configurations
- Uses structure-level operations for fast backtracking
- Treats Sudoku as a mathematical constraint system

This approach is algorithmically elegant and highly robust, making it ideal for demonstrating advanced optimization and data structure design.
