#include "unoptimized/sudoku_unoptimize.hpp"

#include <chrono>
#include <iostream>

long get_micros(const std::chrono::steady_clock::time_point& start,
                const std::chrono::steady_clock::time_point& end) {
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start)
        .count();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "No sudoku file specified\n";
        return 1;
    }

    std::string file_path = argv[1];
    Board board;

    if (read_file(board, file_path) != 0) {
        std::cerr << "Error reading file: " << file_path << "\n";
        return 1;
    }

    Board solution;

    auto start = std::chrono::steady_clock::now();
    int found_solution = solve(board, solution);
    auto end = std::chrono::steady_clock::now();

    long micros = get_micros(start, end);

    if (found_solution) {
        std::cout << "Solution:\n";
        print_board(solution);
        std::cout << "\nTook " << micros << "us\n";
    } else {
        std::cout << "No solution found. Took " << micros << "us\n";
    }

    return 0;
}
