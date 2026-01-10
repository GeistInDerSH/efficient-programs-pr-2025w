## How to compile
```bash
make # Compile all and create sudoku.exe and benchmark.exe
make clean # OPTIONAL -> clean all generated files
```

## How to run
### Solve Sudoku
```bash
./sudoku.exe ../boards/board.sudoku
```
### Run Benchmark
```bash
./benchmark.exe ../boards/board.sudoku
```

### Save Benchmarks to txt
```bash
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
  ./benchmark.exe ../boards/$b
  echo
done > File.txt
```
