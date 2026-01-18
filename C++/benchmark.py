#!/usr/bin/env python3
import csv
import subprocess
import time
from pathlib import Path
from statistics import mean
import re

# -----------------------------
# Configuração
# -----------------------------

RUNS = 5
CPU = "0"
PERF_EVENTS = ["cycles", "instructions"]
OUTCSV = "benchmark_results.csv"

# Lista dos teus solvers
SOLVERS = [
    ("unoptimized", "benchmark_unoptimized.exe"),
    ("bitmasking", "benchmark_bitmaskingrmv.exe"),
    ("bitmasking_fc", "benchmark_bitmaskingrmv_fc.exe"),
    ("dlx", "benchmark_dlx.exe"),
]

# -----------------------------

def _digits_only(s: str) -> str:
    return re.sub(r"[^0-9]", "", s)


def run_perf(solver_bin: Path, solver_name: str, board_path: Path):
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
        solver_name,
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
    script_dir = Path(__file__).resolve().parent
    boards_dir = (script_dir / "../boards").resolve()

    if not boards_dir.exists():
        raise SystemExit(f"Boards dir not found: {boards_dir}")

    boards = sorted(p for p in boards_dir.iterdir() if p.is_file())

    with open(script_dir / OUTCSV, "w", newline="") as f:
        w = csv.writer(f)

        # Cabeçalho CSV
        w.writerow([
            "solver",
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

        for solver_name, bin_name in SOLVERS:
            solver_bin = (script_dir / bin_name).resolve()

            if not solver_bin.exists():
                print(f"Binary not found, skipping: {solver_bin}")
                continue

            for board in boards:
                wall_times = []
                cycles_list = []
                instr_list = []

                for run in range(1, RUNS + 1):
                    print(f"Benchmark for {solver_name} | {board.name} | run {run}")

                    rc, wall_ns, cycles, instr = run_perf(
                        solver_bin, solver_name, board
                    )

                    if rc != 0:
                        print(f"EXIT code: {rc}, skipping run")
                        continue

                    wall_times.append(wall_ns)
                    cycles_list.append(cycles)
                    instr_list.append(instr)

                if not wall_times:
                    print("No valid runs")
                    continue

                w.writerow([
                    solver_name,
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

    print(f"\nBenchmark finished. Wrote: {script_dir / OUTCSV}")


if __name__ == "__main__":
    main()
