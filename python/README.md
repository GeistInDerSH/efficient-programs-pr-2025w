# Python Sudoku Solver

## dataset
The dataset folder contains the text files for testing the solvers.

## solver unoptimized
This implementation is the default implementation, on which the optimization will then be based.

## solver optimized
Here is the optimized implementation.

## How to run
To run the program, use the following command for the unoptimzed version:
```bash
python solver_unoptimized/sudoku_unoptimized.py fully-solved.sudoku
```

To run optimized versions, use the following commands:

For v1, v2, ..., v6 solver, run the following command, by replacing the N in vN with the version number:
```bash
python solver_optimized/sudoku_optimized_vN.py fully-solved.sudoku
```

## v1 optimization
Uses precomputed used sets

## v2 optimization
Uses MRV (Minimum Remaining Values) + Heap

## v3 optimization
Uses Bitmasks

## v4 optimization
Uses Precomputed BOX_MAP & Micro-optimizations, like a globale ALL_NUMS constant

## v5 optimization
Uses Naked-Singles Preprocessing (queue-based propagation)  

## v6 optimization
Uses further micro optimizations