#[macro_use]
extern crate criterion;
use criterion::Criterion;
use sudoku_solver::SudokuSolver;

fn criterion_benchmark(c: &mut Criterion) {
    c.bench_function("read_file exists", |b| {
        b.iter(|| sudoku_solver::read_file("../boards/example-1.sudoku"))
    });
    c.bench_function("read_file dne", |b| {
        b.iter(|| sudoku_solver::read_file("invalid/file.sudoku"))
    });
    c.bench_function("solve for valid board (easy)", |b| {
        b.iter(|| sudoku_solver::example_boards::EASY_BOARD.solve());
    });
    c.bench_function("solve for valid board (medium)", |b| {
        b.iter(|| sudoku_solver::example_boards::MEDIUM_BOARD.solve());
    });
    c.bench_function("solve for valid board (hard)", |b| {
        b.iter(|| sudoku_solver::example_boards::HARD_BOARD.solve());
    });
    c.bench_function("solve for valid board (extra hard)", |b| {
        b.iter(|| sudoku_solver::example_boards::EXTRA_HARD_BOARD.solve());
    });
    c.bench_function("solve for valid board (2x hard)", |b| {
        b.iter(|| sudoku_solver::example_boards::HARD_2X_BOARD.solve());
    });
    c.bench_function("solve for valid board (solved)", |b| {
        b.iter(|| sudoku_solver::example_boards::SOLVED_BOARD.solve());
    });
    c.bench_function("solve for valid board (invalid)", |b| {
        b.iter(|| sudoku_solver::example_boards::INVALID_BOARD.solve());
    });
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
