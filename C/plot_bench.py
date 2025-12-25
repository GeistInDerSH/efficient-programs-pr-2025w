#!/usr/bin/env python3

from __future__ import annotations

import argparse
import os
import re
import sys
from collections import defaultdict

import matplotlib.pyplot as plt

# Accept: "solve_xxx()" or "-----> solve_xxx()" patterns
RE_SOLVER = re.compile(r"^\s*(?:-+>+\s*)?(solve_[A-Za-z0-9_]+)\(\)\s*$")
RE_LINE = re.compile(r"^\s*(Solver Only|Full Run)\s*-\s*(.*?)\s*:\s*([0-9_]+)\s*ns/iter\s*$")

MODES = ("Solver Only", "Full Run")


def parse_text(text: str):
    """
    Returns:
      alg_order: list[str]
      boards_by_mode: dict[mode] -> list[str] (first-seen order for that mode)
      data: dict[mode][alg][board] = int(ns)
    """
    data = {m: defaultdict(dict) for m in MODES}
    boards_by_mode = {m: [] for m in MODES}
    alg_order: list[str] = []

    current_alg: str | None = None

    def add_board(mode: str, board: str):
        if board not in boards_by_mode[mode]:
            boards_by_mode[mode].append(board)

    for raw in text.splitlines():
        line = raw.rstrip("\n")

        m_alg = RE_SOLVER.match(line)
        if m_alg:
            current_alg = m_alg.group(1)
            if current_alg not in alg_order:
                alg_order.append(current_alg)
            continue

        m = RE_LINE.match(line)
        if not m or current_alg is None:
            continue

        mode, board, ns_s = m.group(1), m.group(2).strip(), m.group(3)
        ns = int(ns_s.replace("_", ""))

        add_board(mode, board)
        data[mode][current_alg][board] = ns

    return alg_order, boards_by_mode, data


def make_plot(mode: str, alg_order, boards, data, outpath: str, logy: bool):
    x = list(range(len(boards)))

    plt.figure(figsize=(14, 7))
    for alg in alg_order:
        y = []
        for b in boards:
            # If missing for a specific algorithm/board, leave a gap (NaN)
            y.append(data[mode].get(alg, {}).get(b, float("nan")))
        plt.plot(x, y, marker="o", linewidth=2, label=alg)

    plt.xticks(x, boards, rotation=35, ha="right")
    plt.ylabel("Time (ns/iter)")
    plt.title(f"Sudoku benchmark — {mode}")
    plt.grid(True, which="both", linestyle="--", linewidth=0.6, alpha=0.6)
    if logy:
        plt.yscale("log")

    plt.legend(loc="center left", bbox_to_anchor=(1.02, 0.5), borderaxespad=0.0)
    plt.tight_layout()
    plt.savefig(outpath, dpi=200)
    plt.close()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="infile", default=None, help="Input text file (default: stdin)")
    ap.add_argument("--outdir", default=".", help="Output directory")
    ap.add_argument("--logy", action="store_true", help="Use log scale for Y axis")
    args = ap.parse_args()

    if args.infile:
        with open(args.infile, "r", encoding="utf-8") as f:
            text = f.read()
    else:
        text = sys.stdin.read()

    alg_order, boards_by_mode, data = parse_text(text)

    if not alg_order:
        print("No solver headers found (expected lines like: solve_optimized_v2()).", file=sys.stderr)
        sys.exit(1)

    os.makedirs(args.outdir, exist_ok=True)

    out_solver = os.path.join(args.outdir, "solver_only.png")
    out_full = os.path.join(args.outdir, "full_run.png")

    if not boards_by_mode["Solver Only"]:
        print("No 'Solver Only - ...' lines found.", file=sys.stderr)
    else:
        make_plot("Solver Only", alg_order, boards_by_mode["Solver Only"], data, out_solver, args.logy)
        print(f"Wrote: {out_solver}")

    if not boards_by_mode["Full Run"]:
        print("No 'Full Run - ...' lines found.", file=sys.stderr)
    else:
        make_plot("Full Run", alg_order, boards_by_mode["Full Run"], data, out_full, args.logy)
        print(f"Wrote: {out_full}")


if __name__ == "__main__":
    main()
