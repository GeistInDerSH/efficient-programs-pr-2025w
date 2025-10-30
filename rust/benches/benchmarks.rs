#[macro_use]
extern crate criterion;
use criterion::Criterion;

fn criterion_benchmark(c: &mut Criterion) {
    c.bench_function("read_file exists", |b| {
        b.iter(|| sudoku_solver::read_file("../boards/example-1.sudoku"))
    });
    c.bench_function("read_file dne", |b| {
        b.iter(|| sudoku_solver::read_file("invalid/file.sudoku"))
    });
}

criterion_group!(benches, criterion_benchmark);
criterion_main!(benches);
