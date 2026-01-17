# Compile & Run Guide

## How to compile

### Build all solvers and benchmarks
    make

### Clean generated files (optional)
    make clean

---

## Generated executables

After compiling, the following executables will be available:

| Executable | Description |
|------------|------------|
| sudoku_unoptimized.exe | Runs the unoptimized solver |
| sudoku_bitmaskingrmv.exe | Runs the optimized solver (Bitmasking + MRV) |
| benchmark_unoptimized.exe | Benchmarks the unoptimized solver |
| benchmark_bitmaskingrmv.exe | Benchmarks the optimized solver |

---

## How to run

### Solve Sudoku (unoptimized)
    ./sudoku_unoptimized.exe ../boards/board.sudoku

### Solve Sudoku (bitmasking + MRV)
    ./sudoku_bitmaskingrmv.exe ../boards/board.sudoku

---

## Run Benchmark

### Benchmark unoptimized solver
    ./benchmark_unoptimized.exe unoptimized ../boards/board.sudoku

### Benchmark optimized solver
    ./benchmark_bitmaskingrmv.exe bitmaskingrmv ../boards/board.sudoku

---

## Save Benchmarks to a text file

    for b in \
    fully-solved.sudoku \
    invalid-col-collision.sudoku \
    solvable-2x-hard.sudoku \
    solvable-extra-hard-1.sudoku \
    invalid-box-collision.sudoku \
    invalid-row-col-collision.sudoku \
    solvable-easy-1.sudoku \
    solvable-hard-1.sudoku \
    invalid-characters.sudoku \
    invalid-row-collision.sudoku \
    solvable-example-1.sudoku \
    solvable-medium-1.sudoku
    do
      echo "===== $b ====="
      ./benchmark_bitmaskingrmv.exe bitmaskingrmv ../boards/$b
      echo
    done > File.txt

---

## Notes

- The benchmark executable supports runtime solver selection:
      ./benchmark_bitmaskingrmv.exe <unoptimized|bitmaskingrmv> <board_file>

- This ensures fair comparisons using the same binary, same input, and same iteration count.
