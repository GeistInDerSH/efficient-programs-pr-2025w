#include <iostream>
#include <chrono>
#include <string>

#include "unoptimized/sudoku_unoptimize.hpp"
#include "bitmaskingrmv/sudoku_bitmasking_rmv.hpp"
#include "bitmaskingrmvfc/sudoku_bitmasking_rmv_fc.hpp"
#include "dlx/sudoku_dlx.hpp"

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

            if (v < 1 || v > 9)
                return false;

            int b = (r / 3) * 3 + (c / 3);

            if (row[r][v] || col[c][v] || box[b][v])
                return false;

            row[r][v] = col[c][v] = box[b][v] = true;

            // Não pode alterar pistas do puzzle
            if (original.cells[idx] != 0 && original.cells[idx] != v)
                return false;
        }
    }
    return true;
}

// --------------------------------------------------
// Enum de solvers

enum class SolverType {
    UNOPTIMIZED,
    BITMASKING,
    BITMASKING_FC,
    DLX
};

const char* solver_name(SolverType t) {
    switch (t) {
        case SolverType::UNOPTIMIZED:    return "Unoptimized";
        case SolverType::BITMASKING:    return "Bitmasking+MRV";
        case SolverType::BITMASKING_FC: return "Bitmasking+MRV+FC";
        case SolverType::DLX:           return "DLX (Algorithm X)";
    }
    return "Unknown";
}

// --------------------------------------------------
// Wrappers (evitam confusão de linking)

static bool solve_unoptimized(const Board& in, Board& out) {
    return solve(in, out) == 1;
}

static bool solve_bitmasking(const Board& in, Board& out) {
    return solve(in, out) == 1;
}

static bool solve_bitmasking_fc(const Board& in, Board& out) {
    return solve(in, out) == 1;
}

static bool solve_dlx(const Board& in, Board& out) {
    return solve(in, out) == 1;
}

// --------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage:\n";
        std::cout << "  ./benchmark.exe <unoptimized|bitmasking|bitmasking_fc|dlx> <board_file>\n";
        return 1;
    }

    std::string solver_arg = argv[1];
    std::string filepath = argv[2];

    SolverType solver;

    if (solver_arg == "unoptimized")
        solver = SolverType::UNOPTIMIZED;
    else if (solver_arg == "bitmasking")
        solver = SolverType::BITMASKING;
    else if (solver_arg == "bitmasking_fc")
        solver = SolverType::BITMASKING_FC;
    else if (solver_arg == "dlx")
        solver = SolverType::DLX;
    else {
        std::cout << "Unknown solver: " << solver_arg << "\n";
        return 1;
    }

    Board input;
    if (read_file(input, filepath) != 0) {
        std::cerr << "Failed to read board: " << filepath << "\n";
        return 1;
    }

    constexpr int ITERS = 50;

    Board solution;

    // --------------------------------------------------
    // Warm-up

    for (int i = 0; i < 5; i++) {
        if (solver == SolverType::UNOPTIMIZED)
            solve_unoptimized(input, solution);
        else if (solver == SolverType::BITMASKING)
            solve_bitmasking(input, solution);
        else if (solver == SolverType::BITMASKING_FC)
            solve_bitmasking_fc(input, solution);
        else
            solve_dlx(input, solution);
    }

    // --------------------------------------------------
    // Medição em bloco

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < ITERS; i++) {
        if (solver == SolverType::UNOPTIMIZED)
            solve_unoptimized(input, solution);
        else if (solver == SolverType::BITMASKING)
            solve_bitmasking(input, solution);
        else if (solver == SolverType::BITMASKING_FC)
            solve_bitmasking_fc(input, solution);
        else
            solve_dlx(input, solution);
    }

    auto end = std::chrono::steady_clock::now();

    // --------------------------------------------------
    // Validação

    bool valid = validate_solution(input, solution);

    auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(
        end - start
    ).count();

    double avg_us = double(total_us) / ITERS;

    // --------------------------------------------------
    // Relatório

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
