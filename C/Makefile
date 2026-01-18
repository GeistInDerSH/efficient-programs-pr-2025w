# Compiler and flags
CC = gcc
CFLAGS = -Wall -march=native  -O3 -Wextra  # Warnings, Optimization, Debug symbols

# Executable names
EXECUTABLE = sudoku_solver
BENCHMARK  = sudoku_bench
BENCHMARK_CSV = sudoku_bench_csv
TESTS      = sudoku_test

# Object files for the library
LIB_OBJS = sudoku.o sudoku_unoptimized.o sudoku_optimized_v0.o sudoku_optimized_v1.o sudoku_optimized_v2.o sudoku_optimized_v3.o sudoku_optimized_v4.o sudoku_optimized_v5.o 

all: $(EXECUTABLE) $(BENCHMARK) $(BENCHMARK_CSV) $(TESTS)

# Link the main executable
$(EXECUTABLE): $(LIB_OBJS) main.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(LIB_OBJS) main.o

# Link the benchmark executable
$(BENCHMARK): $(LIB_OBJS) benchmark.o
	$(CC) $(CFLAGS) -o $(BENCHMARK) $(LIB_OBJS) benchmark.o

$(BENCHMARK_CSV): $(LIB_OBJS) benchmark_csv.o
	$(CC) $(CFLAGS) -o $(BENCHMARK_CSV) $(LIB_OBJS) benchmark_csv.o

$(TESTS): $(LIB_OBJS) test.o
	$(CC) $(CFLAGS) -o $(TESTS) $(LIB_OBJS) test.o

# Generic rule to compile .c files into .o (object) files
%.o: %.c sudoku.h sudoku_optimized_v1.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c sudoku_optimized_v1.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(BENCHMARK) $(BENCHMARK_CSV) $(TESTS) *.o
