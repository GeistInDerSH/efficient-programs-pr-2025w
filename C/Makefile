# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra  # Warnings, Optimization, Debug symbols

# Executable names
EXECUTABLE = sudoku_solver
BENCHMARK = sudoku_bench
UNOPTIMIZED = unoptimized
V0 = v0
V1 = v1
V2 = v2
V3 = v3
V4 = v4
V5 = v5

# Files needed for the single entrypoint programs
UNOPTIMIZED_FILES = sudoku.o sudoku_unoptimized.o single_entrypoint.o
V0_FILES = sudoku.o sudoku_optimized_v0.o single_entrypoint.o
V1_FILES = sudoku.o sudoku_optimized_v1.o single_entrypoint.o
V2_FILES = sudoku.o sudoku_optimized_v2.o single_entrypoint.o
V3_FILES = sudoku.o sudoku_optimized_v3.o single_entrypoint.o
V4_FILES = sudoku.o sudoku_optimized_v4.o single_entrypoint.o
V5_FILES = sudoku.o sudoku_optimized_v5.o single_entrypoint.o

# Object files for the library
LIB_OBJS = sudoku.o \
			sudoku_unoptimized.o \
			sudoku_optimized_v0.o \
			sudoku_optimized_v1.o \
			sudoku_optimized_v2.o \
			sudoku_optimized_v3.o \
			sudoku_optimized_v4.o \
			sudoku_optimized_v5.o

all: $(EXECUTABLE) $(BENCHMARK) $(UNOPTIMIZED) $(V0) $(V1) $(V2) $(V3) $(V4) $(V5)

# Link the main executable
$(EXECUTABLE): $(LIB_OBJS) main.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(LIB_OBJS) main.o

# Link the benchmark executable
$(BENCHMARK): $(LIB_OBJS) benchmark.o
	$(CC) $(CFLAGS) -o $(BENCHMARK) $(LIB_OBJS) benchmark.o

$(UNOPTIMIZED): CFLAGS += -DUNOPTIMIZED
$(UNOPTIMIZED): $(UNOPTIMIZED_FILES)
	rm -f entry.o
	$(CC) $(CFLAGS) -o single_entrypoint.o -c single_entrypoint.c
	$(CC) $(CFLAGS) -o $(UNOPTIMIZED) $(UNOPTIMIZED_FILES)

$(V0): CFLAGS += -DV0
$(V0): $(V0_FILES)
	rm -f entry.o
	$(CC) $(CFLAGS) -o single_entrypoint.o -c single_entrypoint.c
	$(CC) $(CFLAGS) -o $(V0) $(V0_FILES)

$(V1): CFLAGS += -DV1
$(V1): $(V1_FILES)
	rm -f entry.o
	$(CC) $(CFLAGS) -o single_entrypoint.o -c single_entrypoint.c
	$(CC) $(CFLAGS) -o $(V1) $(V1_FILES)

$(V2): CFLAGS += -DV2
$(V2): $(V2_FILES)
	rm -f entry.o
	$(CC) $(CFLAGS) -o single_entrypoint.o -c single_entrypoint.c
	$(CC) $(CFLAGS) -o $(V2) $(V2_FILES)

$(V3): CFLAGS += -DV3
$(V3): $(V3_FILES)
	rm -f entry.o
	$(CC) $(CFLAGS) -o single_entrypoint.o -c single_entrypoint.c
	$(CC) $(CFLAGS) -o $(V3) $(V3_FILES)

$(V4): CFLAGS += -DV4
$(V4): $(V4_FILES)
	rm -f entry.o
	$(CC) $(CFLAGS) -o single_entrypoint.o -c single_entrypoint.c
	$(CC) $(CFLAGS) -o $(V4) $(V4_FILES)

$(V5): CFLAGS += -DV5
$(V5): $(V5_FILES)
	rm -f entry.o
	$(CC) $(CFLAGS) -o single_entrypoint.o -c single_entrypoint.c
	$(CC) $(CFLAGS) -o $(V5) $(V5_FILES)

# Generic rule to compile .c files into .o (object) files
%.o: %.c sudoku.h sudoku_optimized_v1.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c sudoku_optimized_v1.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECUTABLE) $(BENCHMARK) $(UNOPTIMIZED) $(V0) $(V1) $(V2) $(V3) $(V4) $(V5) *.o