#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "sudoku.h"
#include "sudoku_unoptimized.h"
#include "sudoku_optimized_v0.h"
#include "sudoku_optimized_v1.h"
#include "sudoku_optimized_v2.h"
#include "sudoku_optimized_v3.h"
#include "sudoku_optimized_v4.h"
#include "sudoku_optimized_v5.h"

#define OUTCSV "benchmark_results_c.csv"
#define ITERS  100

// helper fct: nanos between two timespec
static long get_nanos(const struct timespec* start, const struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000000000L + (end->tv_nsec - start->tv_nsec);
}

// Returns the file name part (after the last "/" )
static const char* basename_c(const char* path) {
    const char* slash = strrchr(path, '/');
    return slash ? (slash + 1) : path;
}

// store the full path to the executable
static void get_program_path(char out[PATH_MAX]) {
    // Linux: /proc/self/exe points to the running binary
    ssize_t n = readlink("/proc/self/exe", out, PATH_MAX - 1);
    if (n > 0) {
        out[n] = '\0';
        return;
    }
    // fallback
    snprintf(out, PATH_MAX, "unknown_program");
}

static void csv_write_row(FILE* csv,
                          const char* program_path,
                          int opt_index,
                          const char* board_name,
                          const char* mode,
                          long ns_per_iter_avg)
{
    // program,opt_index,board,mode,ns_per_iter_avg
    fprintf(csv, "%s,%d,%s,%s,%ld\n", program_path, opt_index, board_name, mode, ns_per_iter_avg);
}

static void bench_test(void* context, struct Board b) {
    SolverFunc solver_to_run = (SolverFunc)context;

    struct Board solved_board;
    int res = solver_to_run(&b, &solved_board);

    if (res == 1) {

        if (!is_solution_valid(&solved_board)) {
            // fprintf(stderr, "fail!\n");
        }
    }
}

static void benchmark_solver_csv(FILE* csv,
                                const char* program_path,
                                int opt_index,
                                const char* filePath,
                                int iters,
                                void (*func)(void*, struct Board),
                                void* context)
{
    struct timespec start, end;
    long total_nanos = 0;

    struct Board b = (struct Board){0};

    if (read_file(&b, filePath) != 0) {
        return;
    }

    if (!is_board_valid(&b)) {
        return;
    }

    for (int i = 0; i < iters; i++) {
        timespec_get(&start, TIME_UTC);
        func(context, b);
        timespec_get(&end, TIME_UTC);
        total_nanos += get_nanos(&start, &end);
    }

    const char* board_name = basename_c(filePath);

    csv_write_row(csv, program_path, opt_index, board_name, "solver_only", total_nanos / iters);
}

static void benchmark_full_run_csv(FILE* csv,
                                  const char* program_path,
                                  int opt_index,
                                  const char* filePath,
                                  int iters,
                                  void (*func)(void*, struct Board),
                                  void* context)
{
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    for (int i = 0; i < iters; i++) {
        struct Board b = (struct Board){0};

        if (read_file(&b, filePath) != 0) {
            continue;
        }
        if (!is_board_valid(&b)) {
            continue;
        }
        func(context, b);
    }

    timespec_get(&end, TIME_UTC);
    long total_nanos = get_nanos(&start, &end);

    const char* board_name = basename_c(filePath);
    csv_write_row(csv, program_path, opt_index, board_name, "full_run", total_nanos / iters);
}

static void benchmark_runner_csv(FILE* csv,
                                 const char* program_path,
                                 int opt_index,
                                 const char* filePath,
                                 int iters,
                                 void (*func)(void*, struct Board),
                                 void* context)
{
    benchmark_solver_csv(csv, program_path, opt_index, filePath, iters, func, context);
    benchmark_full_run_csv(csv, program_path, opt_index, filePath, iters, func, context);
}

// --------------------- benchmark for cache-optimized Board ---------------------

static void bench_test2(void* context, struct Board_CacheOptimized b) {
    SolverFuncCacheOptimized solver_to_run = (SolverFuncCacheOptimized)context;

    struct Board_CacheOptimized output;
    solver_to_run(&b, &output);
}

static void benchmark_solver_cache_csv(FILE* csv,
                                       const char* program_path,
                                       int opt_index,
                                       const char* filePath,
                                       int iters,
                                       void (*func)(void*, struct Board_CacheOptimized),
                                       void* context)
{
    struct timespec start, end;
    long total_nanos = 0;

    struct Board_CacheOptimized b = (struct Board_CacheOptimized){0};

    if (read_file2(&b, filePath) != 0) {
        return;
    }

    if (!is_board_valid_cache_optimized(&b)) {
        return;
    }

    for (int i = 0; i < iters; i++) {
        timespec_get(&start, TIME_UTC);
        func(context, b);
        timespec_get(&end, TIME_UTC);
        total_nanos += get_nanos(&start, &end);
    }

    const char* board_name = basename_c(filePath);
    csv_write_row(csv, program_path, opt_index, board_name, "solver_only", total_nanos / iters);
}

static void benchmark_full_run_cache_csv(FILE* csv,
                                         const char* program_path,
                                         int opt_index,
                                         const char* filePath,
                                         int iters,
                                         void (*func)(void*, struct Board_CacheOptimized),
                                         void* context)
{
    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    for (int i = 0; i < iters; i++) {
        struct Board_CacheOptimized b = (struct Board_CacheOptimized){0};

        if (read_file2(&b, filePath) != 0) {
            continue;
        }
        if (!is_board_valid_cache_optimized(&b)) {
            continue;
        }
        func(context, b);
    }

    timespec_get(&end, TIME_UTC);
    long total_nanos = get_nanos(&start, &end);

    const char* board_name = basename_c(filePath);
    csv_write_row(csv, program_path, opt_index, board_name, "full_run", total_nanos / iters);
}

static void benchmark_runner_cache_csv(FILE* csv,
                                       const char* program_path,
                                       int opt_index,
                                       const char* filePath,
                                       int iters,
                                       void (*func)(void*, struct Board_CacheOptimized),
                                       void* context)
{
    benchmark_solver_cache_csv(csv, program_path, opt_index, filePath, iters, func, context);
    benchmark_full_run_cache_csv(csv, program_path, opt_index, filePath, iters, func, context);
}

int main(void) {
    char program_path[PATH_MAX];
    get_program_path(program_path);

    FILE* csv = fopen(OUTCSV, "w");
    if (!csv) {
        perror("fopen(" OUTCSV ")");
        return 1;
    }

    // csv file header
    fprintf(csv, "program,opt_index,board,mode,ns_per_iter_avg\n");

    // all boards are hardcoded here.
    const char* boards[] = {
        "../boards/fully-solved.sudoku",
        "../boards/invalid-characters.sudoku",
        "../boards/invalid-box-collision.sudoku",
        "../boards/invalid-col-collision.sudoku",
        "../boards/invalid-row-col-collision.sudoku",
        "../boards/invalid-row-collision.sudoku",
        "../boards/solvable-2x-hard.sudoku",
        "../boards/solvable-easy-1.sudoku",
        "../boards/solvable-example-1.sudoku",
        "../boards/solvable-extra-hard-1.sudoku",
        "../boards/solvable-hard-1.sudoku",
        "../boards/solvable-medium-1.sudoku",
    };

    // dynamically find the number of boards
    const int noBoards = (int)(sizeof(boards) / sizeof(boards[0]));

    for (int i = 0; i < noBoards; i++) {
        benchmark_runner_csv(csv, program_path, 0, boards[i], ITERS, bench_test, (void*)solve_optimized_v0);
        benchmark_runner_csv(csv, program_path, 1, boards[i], ITERS, bench_test, (void*)solve_optimized_v1);
        benchmark_runner_csv(csv, program_path, 2, boards[i], ITERS, bench_test, (void*)solve_optimized_v2);

        // v3 is special (cache-optimized)
        benchmark_runner_cache_csv(csv, program_path, 3, boards[i], ITERS, bench_test2, (void*)solve_optimized_v3);

        benchmark_runner_csv(csv, program_path, 4, boards[i], ITERS, bench_test, (void*)solve_optimized_v4);
        benchmark_runner_csv(csv, program_path, 5, boards[i], ITERS, bench_test, (void*)solve_optimized_v5);
    }

    // 6 = unoptimized
    for (int i = 0; i < noBoards; i++) {
        benchmark_runner_csv(csv, program_path, 6, boards[i], ITERS, bench_test, (void*)solve_unoptimized);
    }

    fclose(csv);

    return 0;
}
