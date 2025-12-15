#[macro_use]
extern crate criterion;

use criterion::{criterion_group, criterion_main};
use criterion::{BenchmarkId, Criterion};
use std::io::{BufRead, BufReader};
use std::time::Duration;
use sudoku_solver::{example_boards, SudokuSolver};

fn file_io(c: &mut Criterion) {
    let mut group = c.benchmark_group("file_io");
    group
        .sample_size(1000)
        .warm_up_time(Duration::from_secs(5))
        .measurement_time(Duration::from_secs(10));

    group.bench_function("read_file exists", |b| {
        b.iter(|| sudoku_solver::read_file("../boards/example-1.sudoku"))
    });
    group.bench_function("read_file dne", |b| {
        b.iter(|| sudoku_solver::read_file("invalid/file.sudoku"))
    });

    group.finish();
}

fn invalid_boards(c: &mut Criterion) {
    let bench_table = [
        (
            "multiple",
            example_boards::INVALID_BOARD_ROW_AND_COL_COLLISION,
        ),
        ("row", example_boards::INVALID_BOARD_ROW_COLLISION),
        ("col", example_boards::INVALID_BOARD_COL_COLLISION),
        ("box", example_boards::INVALID_BOARD_BOX_COLLISION),
    ];

    let mut group = c.benchmark_group("invalid");
    group
        .sample_size(1000)
        .measurement_time(Duration::from_secs(10))
        .warm_up_time(Duration::from_secs(5));

    for (name, board) in &bench_table {
        group.bench_function(format!("{name} collisions"), |b| {
            b.iter(|| board.solve());
        });
    }
    group.finish();
}

fn solvable_boards(c: &mut Criterion) {
    let bench_table = [
        ("easy", example_boards::EASY_BOARD),
        ("medium", example_boards::MEDIUM_BOARD),
        ("hard", example_boards::HARD_BOARD),
        ("extra hard", example_boards::EXTRA_HARD_BOARD),
        ("2x hard", example_boards::HARD_2X_BOARD),
        ("fully solved", example_boards::SOLVED_BOARD),
    ];

    let mut group = c.benchmark_group("solvable");
    group
        .sample_size(1000)
        .warm_up_time(Duration::from_secs(5))
        .measurement_time(Duration::from_secs(10));

    for (name, board) in &bench_table {
        group.bench_function(*name, |b| {
            b.iter(|| board.solve());
        });
    }

    group.finish();
}

fn print_boards(c: &mut Criterion) {
    let mut group = c.benchmark_group("display");
    group.sample_size(1000);

    group.bench_function("solved", |b| {
        let solution = example_boards::SOLVED_BOARD.solve().unwrap();
        b.iter(|| solution.to_string());
    });
    group.bench_function("unsolved", |b| {
        let board = example_boards::EASY_BOARD;
        b.iter(|| board.to_string());
    });
    group.finish()
}

fn full_runs(c: &mut Criterion) {
    let bench_table = [
        ("easy", "../boards/solvable-easy-1.sudoku"),
        ("medium", "../boards/solvable-medium-1.sudoku"),
        ("hard", "../boards/solvable-hard-1.sudoku"),
        ("extra hard", "../boards/solvable-extra-hard-1.sudoku"),
        ("2x hard", "../boards/solvable-2x-hard.sudoku"),
        ("fully solved", "../boards/fully-solved.sudoku"),
    ];

    let mut group = c.benchmark_group("full runs");
    group
        .sample_size(5000)
        .warm_up_time(Duration::from_secs(5))
        .measurement_time(Duration::from_secs(15));

    for (name, file_path) in &bench_table {
        group.bench_with_input(
            BenchmarkId::new("full run", *name),
            file_path,
            |b, file_path| {
                b.iter(|| {
                    let board = sudoku_solver::read_file(file_path).unwrap();
                    let solution = board.solve();
                    let _ = std::hint::black_box(match solution {
                        Some(solution) => format!("Solution:\n{solution}"),
                        None => "No solution".to_string(),
                    });
                });
            },
        );
    }
    group.finish();
}

fn bulk_boards(c: &mut Criterion) {
    let bench_table = [
        ("easy", "extra_boards/easy.sudoku"),
        ("medium", "extra_boards/medium.sudoku"),
        ("hard", "extra_boards/hard.sudoku"),
        ("extra hard", "extra_boards/extra_hard.sudoku"),
        ("2x hard", "extra_boards/2x_hard.sudoku"),
    ];

    let mut group = c.benchmark_group("bulk boards");
    group
        .sample_size(1000)
        .warm_up_time(Duration::from_secs(5))
        .measurement_time(Duration::from_secs(15));

    for (name, file_path) in &bench_table {
        group.bench_function(*name, |b| {
            let boards = {
                let file = std::fs::File::open(file_path).unwrap();
                let reader = BufReader::new(file);
                reader
                    .lines()
                    .map_while(Result::ok)
                    .flat_map(|line| sudoku_solver::Board::try_from(line.as_bytes()))
                    .collect::<Vec<_>>()
            };
            let mut cycles = boards.iter().cycle();
            b.iter(|| {
                for board in cycles.by_ref().take(33) {
                    std::hint::black_box(board.solve());
                }
            });
        });
    }

    group.finish();
}

criterion_group!(io, file_io, print_boards);
criterion_group!(boards, solvable_boards, invalid_boards, bulk_boards);
criterion_group!(full, full_runs);
criterion_main!(io, boards, full);
