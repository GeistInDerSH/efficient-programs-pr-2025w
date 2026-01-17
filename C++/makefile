# ----------------------------
# Compiler
# ----------------------------
CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic

# ----------------------------
# Folders
# ----------------------------
UNOPT_DIR := unoptimized
BITMASK_DIR := bitmaskingrmv

# ----------------------------
# Sources
# ----------------------------
UNOPT_SRC := $(UNOPT_DIR)/sudoku_unoptimize.cpp
UNOPT_HDR := $(UNOPT_DIR)/sudoku_unoptimize.hpp

BITMASK_SRC := $(BITMASK_DIR)/sudoku_bitmasking_rmv.cpp
BITMASK_HDR := $(BITMASK_DIR)/sudoku_bitmasking_rmv.hpp

# ----------------------------
# Objects
# ----------------------------
UNOPT_OBJ := $(UNOPT_SRC:.cpp=.o)
BITMASK_OBJ := $(BITMASK_SRC:.cpp=.o)

# ----------------------------
# Targets
# ----------------------------
all: unoptimized bitmaskingrmv

# ----------------------------
# Sudoku executables
# ----------------------------
unoptimized: main.o $(UNOPT_OBJ)
	$(CXX) $(CXXFLAGS) -o sudoku_unoptimized.exe main.o $(UNOPT_OBJ)

bitmaskingrmv: main.o $(BITMASK_OBJ)
	$(CXX) $(CXXFLAGS) -o sudoku_bitmaskingrmv.exe main.o $(BITMASK_OBJ)

# ----------------------------
# Benchmark executables
# ----------------------------
benchmark_unoptimized: benchmark.o $(UNOPT_OBJ)
	$(CXX) $(CXXFLAGS) -o benchmark_unoptimized.exe benchmark.o $(UNOPT_OBJ)

benchmark_bitmaskingrmv: benchmark.o $(BITMASK_OBJ)
	$(CXX) $(CXXFLAGS) -o benchmark_bitmaskingrmv.exe benchmark.o $(BITMASK_OBJ)

# ----------------------------
# Object rules
# ----------------------------
%.o: %.cpp common/board.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ----------------------------
# Cleanup
# ----------------------------
clean:
	rm -f \
		$(UNOPT_DIR)/*.o \
		$(BITMASK_DIR)/*.o \
		*.o \
		*.exe

.PHONY: all clean unoptimized bitmaskingrmv benchmark_unoptimized benchmark_bitmaskingrmv