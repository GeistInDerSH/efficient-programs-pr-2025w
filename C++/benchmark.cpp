#include <iostream>
#include <chrono>
#include <string>
#include "sudoku_unoptimize.hpp" 

// Benchmark function of solve()
long run_solver_once(const Board& input) {
    Board solution;

    auto start = std::chrono::steady_clock::now();
    solve(input, solution);
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cout << "Usage: ./benchmark <sudoku_file>\n";
        return 1;
    }

    std::string filepath = argv[1];

    // Read the board once
    Board b;
    if (read_file(b, filepath) != 0) {
        std::cout << "Error reading file: " << filepath << "\n";
        return 1;
    }

    const int ITERS = 1000;
    long total_ns = 0;

    std::cout << "Solver benchmark unoptimized (C++)\n";
    std::cout << "Board: " << filepath << "\n";
    std::cout << "Iterations: " << ITERS << "\n\n";

    for (int i = 0; i < ITERS; i++) {
        total_ns += run_solver_once(b);
    }

    long avg = total_ns / ITERS;

    std::cout << "Average time per iteration: " << avg << " ns\n";
    std::cout << "Average time per iteration: " << avg / 1000.0 << " µs\n";
    std::cout << "Average time per iteration: " << avg / 1'000'000.0 << " ms\n";

    return 0;
}
