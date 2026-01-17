#include <iostream>
#include <chrono>
#include <string>

#include "unoptimized/sudoku_unoptimize.hpp"
#include "bitmaskingrmv/sudoku_bitmasking_rmv.hpp"

// --------------------------------------------------
// Wrapper para chamar solver dinamicamente

enum class SolverType {
    UNOPTIMIZED,
    BITMASKINGRMV
};

long run_solver_once(SolverType type, const Board& input) {
    Board solution;

    auto start = std::chrono::steady_clock::now();

    if (type == SolverType::UNOPTIMIZED) {
        solve(input, solution);
    } else {
        solve(input, solution);
    }

    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        end - start
    ).count();
}

// --------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage:\n";
        std::cout << "  ./benchmark.exe <unoptimized|bitmaskingrmv> <board_file>\n";
        return 1;
    }

    std::string solver_name = argv[1];
    std::string filepath = argv[2];

    SolverType solver;

    if (solver_name == "unoptimized") {
        solver = SolverType::UNOPTIMIZED;
    } else if (solver_name == "bitmaskingrmv") {
        solver = SolverType::BITMASKINGRMV;
    } else {
        std::cout << "Unknown solver: " << solver_name << "\n";
        return 1;
    }

    Board board;
    if (read_file(board, filepath) != 0) {
        std::cout << "Error reading file: " << filepath << "\n";
        return 1;
    }

    const int ITERS = 1000;
    long total_ns = 0;

    std::cout << "Solver benchmark\n";
    std::cout << "Solver: " << solver_name << "\n";
    std::cout << "Board: " << filepath << "\n";
    std::cout << "Iterations: " << ITERS << "\n\n";

    for (int i = 0; i < ITERS; i++) {
        total_ns += run_solver_once(solver, board);
    }

    long avg = total_ns / ITERS;

    std::cout << "Average time per iteration:\n";
    std::cout << "  " << avg << " ns\n";
    std::cout << "  " << avg / 1000.0 << " µs\n";
    std::cout << "  " << avg / 1'000'000.0 << " ms\n";

    return 0;
}