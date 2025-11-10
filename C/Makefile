# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -g  # Warnings, Optimization, Debug symbols

# Executable names
EXECUTABLE = sudoku_solver
BENCHMARK = sudoku_bench

# Object files for the library
LIB_OBJS = sudoku.o sudoku_optimized_v1.o sudoku_optimized_v2.o

all: $(EXECUTABLE) $(BENCHMARK)

# Link the main executable
$(EXECUTABLE): $(LIB_OBJS) main.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(LIB_OBJS) main.o

# Link the benchmark executable
$(BENCHMARK): $(LIB_OBJS) benchmark.o
	$(CC) $(CFLAGS) -o $(BENCHMARK) $(LIB_OBJS) benchmark.o

# Generic rule to compile .c files into .o (object) files
%.o: %.c sudoku.h sudoku_optimized_v1.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c sudoku_optimized_v1.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(BENCHMARK) *.o