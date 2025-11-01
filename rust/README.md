# Rust Sudoku Solver

`src/lib.rs` defines a `SudokuSolver` trait. Any module can
add an implementation of that trait on the `Board` struct.
From there, the module can be selectively enabled at compile
time to select the solution to use.

The benefits of this are that the entrypoint doesn't need to
change. However, it does assume that you only enable a single
feature flag at a time. It also makes building and testing
a bit more annoying. To help with that,
`scripts/build-all-feature-binaries.sh` and
`scripts/test-all-features.sh` have been added.

## Running Benchmarks

```bash
# cargo bench --no-default-features --features <feature>
# e.g.
cargo bench --no-default-features --features "grid_bit_masking"
```