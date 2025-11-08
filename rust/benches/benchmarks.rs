#[macro_use]
extern crate criterion;
use criterion::Criterion;
use sudoku_solver::SudokuSolver;

fn file_io(c: &mut Criterion) {
    c.bench_function("read_file exists", |b| {
        b.iter(|| sudoku_solver::read_file("../boards/example-1.sudoku"))
    });
    c.bench_function("read_file dne", |b| {
        b.iter(|| sudoku_solver::read_file("invalid/file.sudoku"))
    });
}

fn invalid_boards(c: &mut Criterion) {
    c.bench_function("invalid (multiple collisions)", |b| {
        b.iter(|| sudoku_solver::example_boards::INVALID_BOARD_ROW_AND_COL_COLLISION.solve());
    });
    c.bench_function("invalid (row collisions)", |b| {
        b.iter(|| sudoku_solver::example_boards::INVALID_BOARD_ROW_COLLISION.solve());
    });
    c.bench_function("invalid (col collisions)", |b| {
        b.iter(|| sudoku_solver::example_boards::INVALID_BOARD_COL_COLLISION.solve());
    });
    c.bench_function("invalid (box collisions)", |b| {
        b.iter(|| sudoku_solver::example_boards::INVALID_BOARD_BOX_COLLISION.solve());
    });
}

fn solvable_boards(c: &mut Criterion) {
    c.bench_function("solvable (easy)", |b| {
        b.iter(|| sudoku_solver::example_boards::EASY_BOARD.solve());
    });
    c.bench_function("solvable (medium)", |b| {
        b.iter(|| sudoku_solver::example_boards::MEDIUM_BOARD.solve());
    });
    c.bench_function("solvable (hard)", |b| {
        b.iter(|| sudoku_solver::example_boards::HARD_BOARD.solve());
    });
    c.bench_function("solvable (extra hard)", |b| {
        b.iter(|| sudoku_solver::example_boards::EXTRA_HARD_BOARD.solve());
    });
    c.bench_function("solvable (2x hard)", |b| {
        b.iter(|| sudoku_solver::example_boards::HARD_2X_BOARD.solve());
    });
    c.bench_function("solvable (solved)", |b| {
        b.iter(|| sudoku_solver::example_boards::SOLVED_BOARD.solve());
    });
}

fn print_boards(c: &mut Criterion) {
    c.bench_function("print (easy)", |b| {
        let solution = sudoku_solver::example_boards::EASY_BOARD.solve().unwrap();
        b.iter(|| {
            let _ = std::hint::black_box(format!("{solution}"));
        });
    });
    c.bench_function("print (medium)", |b| {
        let solution = sudoku_solver::example_boards::MEDIUM_BOARD.solve().unwrap();
        b.iter(|| {
            let _ = std::hint::black_box(format!("{solution}"));
        });
    });
    c.bench_function("print (hard)", |b| {
        let solution = sudoku_solver::example_boards::HARD_BOARD.solve().unwrap();

        b.iter(|| {
            let _ = std::hint::black_box(format!("{solution}"));
        });
    });
    c.bench_function("print (extra hard)", |b| {
        let solution = sudoku_solver::example_boards::EXTRA_HARD_BOARD
            .solve()
            .unwrap();
        b.iter(|| {
            let _ = std::hint::black_box(format!("{solution}"));
        });
    });
    c.bench_function("print solution (2x hard)", |b| {
        let solution = sudoku_solver::example_boards::HARD_2X_BOARD
            .solve()
            .unwrap();
        b.iter(|| {
            let _ = std::hint::black_box(format!("{solution}"));
        });
    });
    c.bench_function("print solution (solved)", |b| {
        let solution = sudoku_solver::example_boards::SOLVED_BOARD.solve().unwrap();
        b.iter(|| {
            let _ = std::hint::black_box(format!("{solution}"));
        });
    });
}

criterion_group!(io, file_io, print_boards);
criterion_group!(boards, solvable_boards, invalid_boards);
criterion_main!(io, boards);
