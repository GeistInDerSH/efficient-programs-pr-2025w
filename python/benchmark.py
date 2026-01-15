#!/usr/bin/env python3
import os
from pathlib import Path
from statistics import mean
from typing import List
import subprocess
import time
import csv

BOARDS_DIR = Path("../boards")
RUNS = 5

PERF_EVENTS = ["cycles", "instructions"]
CPU = "0"

OUTCSV = "benchmark_results.csv"

PROGS = [
    ["python3", "../python/solver_unoptimized/sudoku_unoptimized.py"],
    ["python3", "../python/solver_optimized/sudoku_optimized_v1.py"],
    ["python3", "../python/solver_optimized/sudoku_optimized_v2.py"],
    ["python3", "../python/solver_optimized/sudoku_optimized_v3.py"],
    ["python3", "../python/solver_optimized/sudoku_optimized_v4.py"],
    ["python3", "../python/solver_optimized/sudoku_optimized_v5.py"],
    ["python3", "../python/solver_optimized/sudoku_optimized_v6.py"],
]
def run_perf(prog, board):
    """
    Runs one benchmark using perf stat.
    Returns: (exit_code, wall_ns, cycles, instructions)
    """
    perf_cmd = [
        "perf", "stat",
        "-C", CPU,
        "-e", ",".join(PERF_EVENTS),
        "-x", ",",
        "--",
        *prog,
        str(board),
    ]

    start_ns = time.perf_counter_ns()
    proc = subprocess.run(
        perf_cmd,
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
    )
    end_ns = time.perf_counter_ns()

    wall_ns = end_ns - start_ns

    cycles = 0
    instructions = 0

    for line in proc.stderr.splitlines():
        parts = line.split(",")
        if len(parts) < 3:
            continue
        value, _, event = parts[:3]
        value = value.replace(" ", "")
        if not value.isdigit():
            continue
        if event == "cycles":
            cycles = int(value)
        elif event == "instructions":
            instructions = int(value)

    return proc.returncode, wall_ns, cycles, instructions

def main():
    """
    The main method for benchmarking
    """
    with open(OUTCSV, "w", newline="") as f:
        csv_writer = csv.writer(f)
        csv_writer.writerow([
            "program",
            "board",
            "wall_ns_min",
            "wall_ns_avg",
            "wall_ns_max",
            "cycles_min",
            "cycles_avg",
            "cycles_max",
            "instructions_min",
            "instructions_avg",
            "instructions_max",
        ])
        for prog in PROGS:
            prog_name = " ".join(prog)
            for board in sorted(BOARDS_DIR.iterdir()):
                if not board.is_file():
                    continue
                wall_times = []
                cycles_list = []
                instr_list = []
                for run in range(1, RUNS + 1):
                    print(f"Benchmark for {prog_name} | {board.name} | run {run}")
                    rc, wall_ns, cycles, instr = run_perf(prog, board)
                    if rc != 0:
                        print(f"EXIT code: {rc}, skipping run")
                        continue
                    wall_times.append(wall_ns)
                    cycles_list.append(cycles)
                    instr_list.append(instr)
                if not wall_times:
                    print("No valid runs")
                    continue
                csv_writer.writerow([
                    prog_name,
                    board.name,
                    min(wall_times),
                    int(mean(wall_times)),
                    max(wall_times),
                    min(cycles_list),
                    int(mean(cycles_list)),
                    max(cycles_list),
                    min(instr_list),
                    int(mean(instr_list)),
                    max(instr_list),
                ])

    print(f"\nBenchmark finished")


if __name__ == '__main__':
    main()
