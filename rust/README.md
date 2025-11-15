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

## Overview of Improvements

### `src/basic`

This is a basic Sudoku solver. It doesn't use anything too fancy, and
is a basis for the other improvements to start from.

### `src/basic_std_index`

The `std::ops::Index` and `std::ops::IndexMut` are implemented on
the `Board` struct. This allows for indexing as if it was an array.

This tends to be slightly faster than the one in `src/basic`, likely
due to the compiler being able to inline the struct better.

### `src/bit_masking_v1`

This version adds three arrays of 9 `u16` to track values seen in the
rows, column, and boxes on the `Board`. This helps to avoid calls to
the `is_valid` function. In addition, the `is_solvable_board` function
is rolled into the setup code, and avoids needing extra data structures.

This ended up being a considerable improvement on the `src/basic_std_index`
version.

### `src/bitset_masking_v1`

The whole of a sudoku board can be stored into a single `u128`.
This solution, while somewhat inefficient, is an attempt to store
the values seen in a row/column/box into one of three `u128`.

This is a slight degradation over `src/bit_masking_v1`, but is still
faster than the basic solutions.

### `src/bit_masking_v2`

`src/bit_masking_v1`, while fast, does a lot of extra compare operations
for each row/column/box. Because we will bail out in the case that any of
them are invalid, we can delay checking each of them by using bitwise
and & or operations then checking if the result is non-zero.

This offers a moderate improvement over the version in `src/bit_masking_v1`.

### `src/bit_masking_v3`

This version changes up considerably from `src/bit_masking_v2`.
Instead of tracking the values we have seen in the row/column/box array
as 1s, we set all bits to 1 by default and flip them to 0 once we have
seen them.

Then, if we encounter a 0 in the grid, we mark that spot as a location
to be solved in a Vector. This allows us to skip some intermediate
stack frames that the previous solutions needed to do, along with
some loads and comparisons.

This is a decent improvement over `src/bit_masking_v2`.

#### Extension: `constraint_solve_full_sort`

Because we are storing the locations that still need to be solved,
we can sort these in order from the fewest possible solutions to
the most possible solutions. Then we solve in this order, re-sorting
on each recursive call.

This helps to reduce the number of guesses the solver needs to make,
and as a result backtrack less for faster solutions.

This effectively turns the solving into a kind of constraint solver.

This offers a decent improvement over the base `src/bit_masking_v3`,
and a significant improvement over `src/bit_masking_v2`

#### Extension: `constraint_solve_minimal_sort`

`constraint_solve_full_sort` is a nice speedup, but it requires that
we resort the full array on each sub-call. It would be more effective
to find the best index, and then swap it with the current one. This
has a similar worst case number of compares, but only changes the
position of two elements.

`constraint_solve_minimal_sort` and `constraint_solve_full_sort` are
mutually exclusive, and a compiler error is thrown if an attempt
to use both is made

This is faster when compared to `constraint_solve_full_sort`, by a
small amount.

#### Extension: `unchecked_indexing`

Rust is generally fairly good about avoiding bounds checks in release
code, however some is still inserted into the solver methods. Enabling
this option changes accesses to use `unsafe` code for unchecked
mutable/immutable gets.

This offers only a very slight improvement.