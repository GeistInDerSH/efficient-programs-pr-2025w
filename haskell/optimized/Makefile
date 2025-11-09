# Makefile for Sudoku solver
# Usage:
#   make          # build the solver
#   make clean    # remove build artifacts

GHC       = ghc
SRC       = solver.hs
BIN       = solver
BOARDSDIR = ../../boards
MEASURE   = ../../scripts/measure.sh
RUNS      = 5

# Default build: optimized binary
# -O2 -threaded -rtsoptsflag for optimization
all: $(BIN)

$(BIN): $(SRC)
	$(GHC)  -o $(BIN) $(SRC) 	

bench: $(BIN)
	@echo "Running benchmarks on all Sudoku boards in $(BOARDSDIR)"
	# There are some issues with windows and unix file endings messing up the scripts
	# This is here to prevent it
	@dos2unix -q $(MEASURE) || true
	@for f in $(BOARDSDIR)/*; do \
	  if [ -f "$$f" ]; then \
	    dos2unix -q "$$f" || true; \
	    echo "\n=== Benchmarking $$f ==="; \
	    bash $(MEASURE) ./$(BIN) "$$f" --runs $(RUNS); \
	  fi; \
	done

# Clean up build files
clean:
	rm -f $(BIN) *.hi *.o

.PHONY: all bench clean
