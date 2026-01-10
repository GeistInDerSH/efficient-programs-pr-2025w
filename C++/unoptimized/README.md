# Unoptimized C++ Sudoku Solver (Baseline)

This repository contains an **intentionally unoptimized C++ implementation** of a Sudoku solver.  
The file `sudoku_unoptimized.cpp` represents the **baseline version** used for performance analysis and comparison with future optimized versions.

The goal of this implementation is **not efficiency**, but to provide a **clear and measurable starting point** for studying program optimization techniques.

---

## What Makes This Version Unoptimized

The unoptimized nature of this implementation is **deliberate**. The following design choices contribute to reduced performance:

### • Redundant Memory Accesses
The Sudoku board is accessed repeatedly through recalculated indices and helper functions instead of direct or cached accesses. This increases:
- memory loads
- cache pressure
- execution time

---

### • No Elimination of Repeated Computations
Common expressions (such as index calculations) are recomputed multiple times instead of being stored and reused, increasing arithmetic overhead.

---

### • High Branch Density
Validation logic includes multiple conditional checks that are not strictly necessary, resulting in:
- many conditional branches
- poor branch predictability
- increased branch misprediction penalties

---

### • Function Call Overhead in Hot Paths
Small helper functions are used inside performance-critical code paths, introducing:
- function call overhead
- reduced inlining opportunities
- additional control dependencies

---

### • Naive Recursive Backtracking
The solver uses a straightforward recursive backtracking algorithm without:
- heuristics
- constraint ordering
- aggressive pruning  

This leads to a high number of recursive calls and unnecessary search.

---

### • No Hardware-Aware Optimizations
This version does not exploit:
- data locality
- instruction-level parallelism
- out-of-order execution
- SIMD or vectorization

---

## Purpose of This Baseline

This unoptimized version exists to:

- Serve as a **functional reference implementation**
- Enable **clear performance measurements**
- Highlight the impact of subsequent optimizations
- Provide justification for code transformations based on empirical results

---

## Academic Context

This work follows the methodology presented in *Efficient Programs*
