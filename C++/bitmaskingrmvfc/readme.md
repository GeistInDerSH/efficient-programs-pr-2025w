# Bitmasking + MRV + Forward Checking (FC)

## What Changed Compared to Bitmasking + MRV

This version adds **Forward Checking** on top of bitmasking and the MRV heuristic.

---

## 1. Explicit Cell Domains

### Code Change
Each empty cell now maintains a **domain mask**:
- domain[81]
- Each bit represents a value that is still allowed for that cell.

Domains are initialized once using the row, column, and box masks:
- domain = (~used) & FULL_MASK

### Efficiency Impact
- Avoids recomputing valid values repeatedly
- Makes constraint propagation cheap and local
- Improves cache locality by working on a compact 81-element array

---

## 2. Forward Constraint Propagation

### Code Change
When a value is placed, that value is immediately **removed from the domains of all neighboring cells**:
- Same row
- Same column
- Same 3×3 box

All changes are tracked and undone on backtracking.

### Efficiency Impact
- Detects contradictions immediately
- Prevents exploring branches that are already invalid
- Reduces recursion depth and total search space
- Especially effective on harder puzzles

---

## 3. MRV Uses Domain Size Instead of Masks

### Code Change
The MRV heuristic now selects the cell with the **smallest domain size** instead of recomputing availability from row/column/box masks.

### Efficiency Impact
- Faster cell selection
- More accurate constraint awareness
- Better pruning of the search tree

---

## Overall Performance Effect

Compared to Bitmasking + MRV:
- Fewer recursive calls
- Earlier detection of dead ends
- Lower instruction count per solution
- More stable runtime across puzzle difficulties

---

## Summary

This version transforms the solver from a heuristic-driven search into a **constraint-propagation system**, where most invalid states are eliminated before deep recursion happens.

It significantly improves performance consistency and reduces unnecessary backtracking.
