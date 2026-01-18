#include <iostream>
#include <chrono>
#include <string>
#include <vector>

#include "unoptimized/sudoku_unoptimize.hpp"
#include "bitmaskingrmv/sudoku_bitmasking_rmv.hpp"

// --------------------------------------------------
// Validação de solução Sudoku

static bool validate_solution(const Board& original, const Board& solved) {
    bool row[9][10] = {};
    bool col[9][10] = {};
    bool box[9][10] = {};

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int idx = r * 9 + c;
            int v = solved.cells[idx];

            if (v < 1 || v > 9) return false;

            int b = (r / 3) * 3 + (c / 3);

            if (row[r][v] || col[c][v] || box[b][v])
                return false;

            row[r][v] = col[c][v] = box[b][v] = true;

            // Não pode alterar células fixas do puzzle
            if (original.cells[idx] != 0 && original.cells[idx] != v)
                return false;
        }
    }
    return true;
}

// --------------------------------------------------
// Wrappers separados para evitar confusão de linking

static bool solve_unoptimized(const Board& in, Board& out) {
    return solve(in, out) == 1;
}

static bool solve_bitmasking(const Board& in, Board& out) {
    return solve(in, out) == 1;
}

// --------------------------------------------------

enum class SolverType {
    UNOPTIMIZED,
    BITMASKING
};

const char* solver_name(SolverType t) {
    return t == SolverType::UNOPTIMIZED ? "Unoptimized" : "Bitmasking+MRV";
}

// --------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage:\n";
        std::cout << "  ./benchmark.exe <unoptimized|bitmasking> <board_file>\n";
        return 1;
    }

    std::string solver_arg = argv[1];
    std::string filepath = argv[2];

    SolverType solver;

    if (solver_arg == "unoptimized")
        solver = SolverType::UNOPTIMIZED;
    else if (solver_arg == "bitmasking")
        solver = SolverType::BITMASKING;
    else {
        std::cout << "Unknown solver: " << solver_arg << "\n";
        return 1;
    }

    Board input;
    if (read_file(input, filepath) != 0) {
        std::cerr << "Failed to read board: " << filepath << "\n";
        return 1;
    }

    constexpr int ITERS = 50; // Ajusta para puzzles fáceis/difíceis

    Board solution;

    // --------------------------------------------------
    // Warm-up (cache, branch predictor, JIT interno do SO)
    for (int i = 0; i < 5; i++) {
        if (solver == SolverType::UNOPTIMIZED)
            solve_unoptimized(input, solution);
        else
            solve_bitmasking(input, solution);
    }

    // --------------------------------------------------
    // Medição em bloco (evita erro de resolução do relógio)

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < ITERS; i++) {
        if (solver == SolverType::UNOPTIMIZED)
            solve_unoptimized(input, solution);
        else
            solve_bitmasking(input, solution);
    }

    auto end = std::chrono::steady_clock::now();

    // --------------------------------------------------
    // Validação da última solução

    bool valid = validate_solution(input, solution);

    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(
        end - start
    ).count();

    double avg_us = double(total_us) / ITERS;

    std::cout << "Benchmark report\n";
    std::cout << "-----------------------------\n";
    std::cout << "Solver     : " << solver_name(solver) << "\n";
    std::cout << "Board file : " << filepath << "\n";
    std::cout << "Iterations : " << ITERS << "\n\n";

    std::cout << "Total time : " << total_us << " us\n";
    std::cout << "Avg / run  : " << avg_us << " us\n";
    std::cout << "Avg / run  : " << avg_us / 1000.0 << " ms\n\n";

    std::cout << "Solution valid: " << (valid ? "YES" : "NO") << "\n";

    return valid ? 0 : 2;
}
