#!/usr/bin/env python3
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.ticker as mtick
from matplotlib.gridspec import GridSpec

filename = 'data/benchmark_results.csv'


def plot_csv_results():
    df = pd.read_csv(filename)
    boards = [
        "fully-solved.sudoku",
        "solvable-easy-1.sudoku",
        "solvable-medium-1.sudoku",
        "solvable-hard-1.sudoku",
        "solvable-extra-hard-1.sudoku",
        "solvable-2x-hard.sudoku",
    ]
    df = df[df["board"].isin(boards)]
    df["board"] = pd.Categorical(
        df["board"],
        categories=boards,
        ordered=True
    )

    df["program_short"] = df["program"].str.removeprefix('solve_')

    order = ["basic",
             "basic_std_index",
             "bit_masking_v1",
             "bit_masking_v1_si",
             "bit_masking_v2",
             "bit_masking_v2_si",
             "bitset_masking_v1",
             "bit_masking_v3",
             "bit_masking_v3_cs_full",
             "bit_masking_v3_cs_full_ui",
             "bit_masking_v3_cs_min",
             "bit_masking_v3_cs_min_ui",
             "bit_masking_v4",
             "bit_masking_v4_ui"]
    df["program_short"] = pd.Categorical(df["program_short"], categories=order, ordered=True)

    metrics = ["wall_ns_avg", "cycles_avg", "instructions_avg"]
    titles = ["Wall time ns (avg)", "CPU cycles (avg)", "Instructions (avg)"]

    fig = plt.figure(figsize=(16, 10))
    gs = GridSpec(2, 2, figure=fig)
    axes = [
        fig.add_subplot(gs[0, 0]),
        fig.add_subplot(gs[0, 1]),
        fig.add_subplot(gs[1, 0])
    ]

    for ax, metric, title in zip(axes, metrics, titles):
        pivot = df.pivot_table(
            index="program_short", columns="board", values=metric, aggfunc="mean"
        )
        pivot.plot(kind="bar", ax=ax)
        ax.set_xticklabels(ax.get_xticklabels(), rotation=30, ha="right")
        ax.set_xlabel("")
        ax.set_ylabel(title)
        ax.yaxis.set_major_formatter(mtick.StrMethodFormatter("{x:,.0f}"))
        ax.set_title(title)
        ax.legend(loc='upper center', bbox_to_anchor=(0.5, 1.0), ncol=2)

    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, title="Sudoku puzzle", bbox_to_anchor=(1.05, 0.95), loc='upper left')

    plt.tight_layout()
    plt.show()

def main():
    plot_csv_results()

if __name__ == '__main__':
    main()

