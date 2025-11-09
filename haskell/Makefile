# Makefile for Sudoku solver
# Usage:
#   make          # build the solver
#   make clean    # remove build artifacts

GHC       = ghc
SRC       = solver.hs
BIN       = solver

# Default build: optimized binary
all: $(BIN)

$(BIN): $(SRC)
	$(GHC) -O2 -threaded -rtsopts -o $(BIN) $(SRC)

# Clean up build files
clean:
	rm -f $(BIN) *.hi *.o

.PHONY: all run clean
