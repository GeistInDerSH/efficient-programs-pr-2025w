#!/usr/bin/env python3
import os
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import matplotlib.ticker as mtick
from matplotlib.gridspec import GridSpec

filename = 'benchmark_results.csv'


def plot_csv_results():
    df = pd.read_csv(filename)
    boards = [
        "fully-solved.sudoku",
        "solvable-easy-1.sudoku",
        "solvable-medium-1.sudoku",
        "solvable-example-1.sudoku",
        "solvable-hard-1.sudoku",
        "solvable-2x-hard.sudoku",
        "solvable-extra-hard-1.sudoku"
    ]
    df = df[df["board"].isin(boards)]

    df["program_short"] = np.where(
        df["program"].str.contains("unoptimized"),
        "unoptimized",
        "v" + df["program"].str.extract(r"_v(\d+)")[0]
    )

    df = df[df["program_short"] != "v6"]

    order = ["unoptimized", "v1", "v2", "v3", "v4", "v5"]
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
        ax.set_xticklabels(ax.get_xticklabels(), rotation=30)
        ax.set_xlabel("")
        ax.set_ylabel(title)
        ax.yaxis.set_major_formatter(mtick.StrMethodFormatter("{x:,.0f}"))
        ax.set_title(title)

    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, title="Sudoku puzzle", bbox_to_anchor=(1.05, 0.95), loc='upper left')

    plt.tight_layout()
    plt.show()

def main():
    plot_csv_results()

if __name__ == '__main__':
    main()

