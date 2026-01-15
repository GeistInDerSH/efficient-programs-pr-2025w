#!/usr/bin/env python3
import csv
import pathlib
import re
import statistics
import subprocess
import time

RUNS = 5 # Change this by need
CPU = "0"
PERF_EVENTS = ["cycles", "instructions"]
OUTPUT_CSV = "benchmark_results.csv"

def _digits_only(s: str) -> str:
    # perf can output values with separators or "<not counted>"
    return re.sub(r"[^0-9]", "", s)

def run_perf(solver_bin: pathlib.Path, board_path: pathlib.Path):
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
        str(solver_bin),
        str(board_path),
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
        value_raw, _, event = parts[:3]
        event = event.strip()
        value = _digits_only(value_raw)
        if not value:
            continue

        if event == "cycles":
            cycles = int(value)
        elif event == "instructions":
            instructions = int(value)

    return proc.returncode, wall_ns, cycles, instructions

def main():
    script_dir = pathlib.Path(__file__).resolve().parent
    boards_dir = script_dir.joinpath("../../boards").resolve()
    solver_bins = list(script_dir.parent.glob('solve_*'))
    if not solver_bins:
        raise SystemExit(f"No solver binaries found, first run {script_dir.joinpath('build-all-feature-binaries.sh').resolve()}")

    if not boards_dir.exists():
        raise SystemExit(f"Boards dir not found: {boards_dir}")

    boards = sorted([p for p in boards_dir.iterdir() if p.is_file()])

    output_file = script_dir.joinpath(f'../data/{OUTPUT_CSV}')
    with open(output_file, "w", newline="") as f:
        w = csv.writer(f)

        # The table header 
        w.writerow([
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

        for program in solver_bins:
            for board in boards:
                wall_times = []
                cycles_list = []
                instr_list = []

                for run in range(1, RUNS + 1):
                    print(f"Benchmark for {program.name} | {board.name} | run {run}")
                    rc, wall_ns, cycles, instr = run_perf(program, board)

                    if rc != 0:
                        print(f"EXIT code: {rc}, skipping run")
                        continue

                    wall_times.append(wall_ns)
                    cycles_list.append(cycles)
                    instr_list.append(instr)

                if not wall_times:
                    print("No valid runs")
                    continue

                # Write a row into the csv file
                w.writerow([
                    program.name,
                    board.name,
                    min(wall_times),
                    int(statistics.mean(wall_times)),
                    max(wall_times),
                    min(cycles_list),
                    int(statistics.mean(cycles_list)),
                    max(cycles_list),
                    min(instr_list),
                    int(statistics.mean(instr_list)),
                    max(instr_list),
                ])

    print(f"\nBenchmark finished. Wrote: {output_file}")


if __name__ == "__main__":
    main()
