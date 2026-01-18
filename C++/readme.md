# Sudoku Solver — Build & Benchmark Guide

This project contains multiple Sudoku solvers implemented in C++:
- Unoptimized Backtracking
- Bitmasking + Minimum Remaining Values (MRV)
- Bitmasking + MRV + Forward Checking (FC)
- Dancing Links (Algorithm X / DLX)
- Hybrid (MRV + FC + LCV + Bitmasking)

Benchmarks are performed using Linux `perf` and a Python automation script.

---

## Requirements

- Linux
- g++ (C++17 compatible)
- Python 3
- Linux perf

Install dependencies (Ubuntu/Debian):
```bash
sudo apt install g++ python3 linux-tools-common linux-tools-generic
```

## Build Instructions

From the project root:

```bash
make clean

make unoptimized
make bitmaskingrmv
make bitmaskingrmv_fc
make dlx
make hybrid
```

this will generate:

```bash
sudoku_unoptimized.exe
sudoku_bitmaskingrmv.exe
sudoku_bitmaskingrmv_fc.exe
sudoku_dlx.exe
sudoku_hybrid.exe
```

## Running a solver

Syntax:

```bash
./<solver_binary> <board_file>
```

Example:

```bash
./sudoku_hybrid.exe ../boards/solvable-easy-1.sudoku
```

This will print the solved Sudoku to the terminal.

## Benchmarking (Automated with perf)

1. Make the script executable

```bash
chmod +x benchmark.py
```

2. Run all benchmarks

```bash
./benchmark.py
```

The script will:

- Run each solver on all boards in ../boards/
- Execute each test multiple times
- Collect:
    - Wall-clock time (ns)
    - CPU cycles
    - Retired instructions
- Write results to:
```bash
benchmark_results.csv
```
