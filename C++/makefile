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
BITMASK_FC_DIR := bitmaskingrmvfc

# ----------------------------
# Sources
# ----------------------------
UNOPT_SRC := $(UNOPT_DIR)/sudoku_unoptimize.cpp
UNOPT_HDR := $(UNOPT_DIR)/sudoku_unoptimize.hpp

BITMASK_SRC := $(BITMASK_DIR)/sudoku_bitmasking_rmv.cpp
BITMASK_HDR := $(BITMASK_DIR)/sudoku_bitmasking_rmv.hpp

BITMASK_FC_SRC := $(BITMASK_FC_DIR)/sudoku_bitmasking_rmv_fc.cpp
BITMASK_FC_HDR := $(BITMASK_FC_DIR)/sudoku_bitmasking_rmv_fc.hpp

# ----------------------------
# Objects
# ----------------------------
UNOPT_OBJ := $(UNOPT_SRC:.cpp=.o)
BITMASK_OBJ := $(BITMASK_SRC:.cpp=.o)
BITMASK_FC_OBJ := $(BITMASK_FC_SRC:.cpp=.o)

# ----------------------------
# Targets
# ----------------------------
all: unoptimized bitmaskingrmv bitmaskingrmv_fc

# ----------------------------
# Sudoku executables
# ----------------------------
unoptimized: main.o $(UNOPT_OBJ)
	$(CXX) $(CXXFLAGS) -o sudoku_unoptimized.exe main.o $(UNOPT_OBJ)

bitmaskingrmv: main.o $(BITMASK_OBJ)
	$(CXX) $(CXXFLAGS) -o sudoku_bitmaskingrmv.exe main.o $(BITMASK_OBJ)

bitmaskingrmv_fc: main.o $(BITMASK_FC_OBJ)
	$(CXX) $(CXXFLAGS) -o sudoku_bitmaskingrmv_fc.exe main.o $(BITMASK_FC_OBJ)

# ----------------------------
# Benchmark executables
# ----------------------------
benchmark_unoptimized: benchmark.o $(UNOPT_OBJ)
	$(CXX) $(CXXFLAGS) -o benchmark_unoptimized.exe benchmark.o $(UNOPT_OBJ)

benchmark_bitmaskingrmv: benchmark.o $(BITMASK_OBJ)
	$(CXX) $(CXXFLAGS) -o benchmark_bitmaskingrmv.exe benchmark.o $(BITMASK_OBJ)

benchmark_bitmaskingrmv_fc: benchmark.o $(BITMASK_FC_OBJ)
	$(CXX) $(CXXFLAGS) -o benchmark_bitmaskingrmv_fc.exe benchmark.o $(BITMASK_FC_OBJ)

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
		$(BITMASK_FC_DIR)/*.o \
		*.o \
		*.exe \
		bench_*

.PHONY: all clean unoptimized bitmaskingrmv bitmaskingrmv_fc \
        benchmark_unoptimized benchmark_bitmaskingrmv benchmark_bitmaskingrmv_fc
