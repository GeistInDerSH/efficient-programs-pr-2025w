#[macro_use]
extern crate criterion;
use criterion::Criterion;
use sudoku_solver::{example_boards, SudokuSolver};

fn file_io(c: &mut Criterion) {
    c.bench_function("read_file exists", |b| {
        b.iter(|| sudoku_solver::read_file("../boards/example-1.sudoku"))
    });
    c.bench_function("read_file dne", |b| {
        b.iter(|| sudoku_solver::read_file("invalid/file.sudoku"))
    });
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
    for (name, board) in &bench_table {
        c.bench_function(&format!("invalid ({name} collisions)"), |b| {
            b.iter(|| board.solve());
        });
    }
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

    for (name, board) in &bench_table {
        c.bench_function(&format!("solvable ({name})"), |b| {
            b.iter(|| board.solve());
        });
    }
}

fn print_boards(c: &mut Criterion) {
    let bench_table = [
        ("easy", example_boards::EASY_BOARD),
        ("medium", example_boards::MEDIUM_BOARD),
        ("hard", example_boards::HARD_BOARD),
        ("extra hard", example_boards::EXTRA_HARD_BOARD),
        ("2x hard", example_boards::HARD_2X_BOARD),
        ("fully solved", example_boards::SOLVED_BOARD),
    ];

    for (name, board) in &bench_table {
        c.bench_function(&format!("print ({name})"), |b| {
            let solution = board.solve().unwrap();
            b.iter(|| {
                let _ = std::hint::black_box(format!("{solution}"));
            });
        });
    }
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

    for (name, file_path) in &bench_table {
        c.bench_function(&format!("full ({name})"), |b| {
            b.iter(|| {
                let board = sudoku_solver::read_file(file_path).unwrap();
                let solution = board.solve();
                let _ = std::hint::black_box(match solution {
                    Some(solution) => format!("{solution}"),
                    None => "No solution".to_string(),
                });
            })
        });
        c.bench_function(&format!("full no display ({name})"), |b| {
            b.iter(|| {
                let board = sudoku_solver::read_file(file_path).unwrap();
                let _ = std::hint::black_box(board.solve());
            })
        });
    }
}

criterion_group!(io, file_io, print_boards);
criterion_group!(boards, solvable_boards, invalid_boards);
criterion_group!(full, full_runs);
criterion_main!(io, boards, full);
