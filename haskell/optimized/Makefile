# Makefile for Sudoku solver (Cabal-based)

BIN        = solver
BOARDSDIR = ../../boards
MEASURE   = ../../scripts/measure.sh
RUNS      = 5

# Cabal executable name
CABAL_EXE = sudoku-solver

all:
	cabal build

$(BIN): all
	cp $$(cabal list-bin $(CABAL_EXE)) ./$(BIN)

bench: $(BIN)
	@echo "Running benchmarks on all Sudoku boards in $(BOARDSDIR)"
	@dos2unix -q $(MEASURE) || true
	@for f in $(BOARDSDIR)/*; do \
	  if [ -f "$$f" ]; then \
	    dos2unix -q "$$f" || true; \
	    echo "\n=== Benchmarking $$f ==="; \
	    bash $(MEASURE) ./$(BIN) "$$f" --runs $(RUNS); \
	  fi; \
	done

clean:
	cabal clean
	rm -f $(BIN)

.PHONY: all bench clean