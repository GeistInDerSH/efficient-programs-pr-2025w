# c++ Compiler
CXX := g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pedantic

# Source Files
SOURCES  = unoptimized/sudoku_unoptimize.cpp
HEADERS  = unoptimized/sudoku_unoptimize.hpp

# Common objects
OBJECTS  = $(SOURCES:.cpp=.o)

# Primary targets
all: sudoku benchmark

# Main program that solves a sudoku puzzle
sudoku: main.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o sudoku.exe main.o $(OBJECTS)

# Benchmark program
benchmark: benchmark.o $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o benchmark.exe benchmark.o $(OBJECTS)

# General rules for .o
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Delete generated files
clean:
	rm -f *.o *.exe

.PHONY: all clean sudoku benchmark