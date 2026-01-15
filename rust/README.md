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

#### Extension: `skip_intermediate_stack_frames`

As was noted via reading through the C implementation of this version,
we can attempt to speed up the program by calculating what the next
row and column will be before recurring into the solve function. This
helps avoid extra stack frames where these values are only incremented.

This offers a small performance boost.

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

#### Extension: `skip_intermediate_stack_frames`

As was noted via reading through the C implementation of this version,
we can attempt to speed up the program by calculating what the next
row and column will be before recurring into the solve function. This
helps avoid extra stack frames where these values are only incremented.

This offers a small performance boost.

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

### `src/bit_masking_v4`

This is largely very similar to `src/bit_masking_v3` with the
`constraint_solve_minimal_sort` feature enabled but with several
key improvements.

1) Avoid needing to decrement the value when validating the board
2) Use `u16::wrapping_neg` over `0 - u16`
3) Model the different states for validating an incoming board (solved, unsolved, unsolvable)
    1) As part of this, some struct construction can be delayed in some cases
4) Re-order some data load instructions to make better use of caching

In addition to those, the code using safe and unsafe Rust are split into
two modules that cannot be mutually imported.

## Global Extensions

### `measure_runtime`

This instruments `src/main.rs` to measure how long it took for the program to run.
This also allows for a 2nd argument to be given to the program to determine how many
runs to do for the timing.

This has no performance impact, and is just used for benchmarking and exploration.

### `unchecked_indexing`

Rust is generally fairly good about avoiding bounds checks in release
code, however some is still inserted into the solver methods. Enabling
this option changes accesses to use `unsafe` code for unchecked
mutable/immutable gets.

This offers only a very slight improvement.

### `raw_syscalls`

`cargo flamegraph` showed that when the number of runs is large, there is a
lot of time spend in the read syscall. This attempts to fix that by generating
AMD64 Linux assembly inline to make the Open/Read/Write syscalls.

The benefit is basically none when running it via the CLI, but is noticeable
when running with `measure_runtime` because it can do many runs with file system IO.

## Data

All data was collected on the following machine:

``` 
CPU: Intel(R) Xeon(R) E-2388G CPU @ 3.20GHz
RAM: 125Gi
```

All times are shown in nanoseconds.

### Full Runs

| Features                                                               | easy   | medium | hard    | extra hard | 2x hard   | fully solved | 
|------------------------------------------------------------------------|--------|--------|---------|------------|-----------|--------------|
| solve_basic                                                            | 6_734  | 13_273 | 298_981 | 962_511    | 3_699_895 | 7_369        |
| solve_basic_std_index                                                  | 6_607  | 11_234 | 229_345 | 741_058    | 2_930_615 | 7_466        |
| solve_basic_std_index unchecked_indexing                               | 6_549  | 11_184 | 228_611 | 735_927    | 2_906_218 | 7_444        |
| solve_bit_masking_v1                                                   | 4_498  | 7_884  | 117_644 | 388_753    | 1_490_485 | 3_943        |
| solve_bit_masking_v1 unchecked_indexing                                | 4_462  | 7_298  | 124_602 | 405_178    | 1_451_428 | 3_905        |
| solve_bit_masking_v1 skip_intermediate_stack_frames                    | 4_669  | 8_045  | 120_447 | 404_021    | 1_555_590 | 4_073        |
| solve_bit_masking_v1 skip_intermediate_stack_frames unchecked_indexing | 4_513  | 7_962  | 119_785 | 403_568    | 1_553_745 | 3_972        |
| solve_bit_masking_v2                                                   | 4_529  | 7_592  | 96_120  | 314_033    | 1_293_771 | 3_962        |
| solve_bit_masking_v2 unchecked_indexing                                | 4_496  | 7_602  | 99_272  | 320_379    | 1_294_014 | 3_985        |
| solve_bit_masking_v2 skip_intermediate_stack_frames                    | 4_658  | 8_585  | 123_978 | 424_833    | 1_682_062 | 4_089        |
| solve_bit_masking_v2 skip_intermediate_stack_frames unchecked_indexing | 4_627  | 8_754  | 127_980 | 436_373    | 1_749_564 | 4_032        |
| solve_bitset_masking_v1                                                | 5_109  | 11_266 | 208_034 | 653_951    | 2_597_070 | 4_185        |
| solve_bitset_masking_v1 unchecked_indexing                             | 5_112  | 11_150 | 221_794 | 671_824    | 2_576_787 | 4_191        |
| solve_bit_masking_v3                                                   | 4_402  | 5_473  | 33_568  | 100_645    | 426_394   | 3_946        |
| solve_bit_masking_v3 unchecked_indexing                                | 4_524  | 5_605  | 34_576  | 119_027    | 448_720   | 4_004        |
| solve_bit_masking_v3 constraint_solve_full_sort                        | 10_086 | 10_246 | 14_478  | 14_542     | 37_585    | 3_996        |
| solve_bit_masking_v3 constraint_solve_full_sort unchecked_indexing     | 9_124  | 9_257  | 13_124  | 13_026     | 33_366    | 3_993        |
| solve_bit_masking_v3 constraint_solve_minimal_sort                     | 4_552  | 4_846  | 5_109   | 5_304      | 5_557     | 3_937        |
| solve_bit_masking_v3 constraint_solve_minimal_sort unchecked_indexing  | 4_594  | 4_869  | 5_195   | 5_375      | 5_655     | 3_997        |
| solve_bit_masking_v4                                                   | 4_378  | 4_704  | 5_026   | 5_231      | 5_472     | 4_002        |
| solve_bit_masking_v4 unchecked_indexing                                | 4_416  | 4_674  | 4_848   | 5_103      | 5_275     | 4_070        |

### Solver Only

| Features                                                               | easy  | medium | hard    | extra hard | 2x hard   | fully solved | multiple collisions (invalid) | row collisions (invalid) | col collisions (invalid) | box collisions (invalid) |
|------------------------------------------------------------------------|-------|--------|---------|------------|-----------|--------------|-------------------------------|--------------------------|--------------------------|--------------------------|
| solve_basic                                                            | 2_711 | 9_191  | 294_697 | 949_739    | 3_711_520 | 3_398        | 430                           | 421                      | 1_127                    | 1_632                    |
| solve_basic_std_index                                                  | 2_400 | 7_051  | 223_543 | 734_755    | 2_931_659 | 3_644        | 417                           | 422                      | 1_118                    | 1_636                    |
| solve_basic_std_index unchecked_indexing                               | 2_473 | 7_115  | 224_141 | 731_327    | 2_923_470 | 3_415        | 443                           | 447                      | 1_162                    | 1_688                    |
| solve_bit_masking_v1                                                   | 638   | 4_045  | 118_158 | 391_683    | 1_485_172 | 100          | 16                            | 13                       | 27                       | 57                       |
| solve_bit_masking_v1 unchecked_indexing                                | 559   | 3_338  | 120_441 | 401_624    | 1_449_104 | 104          | 15                            | 13                       | 26                       | 57                       |
| solve_bit_masking_v1 skip_intermediate_stack_frames                    | 655   | 4_134  | 122_011 | 411_946    | 1_546_460 | 157          | 14                            | 15                       | 27                       | 58                       |
| solve_bit_masking_v1 skip_intermediate_stack_frames unchecked_indexing | 649   | 4_005  | 115_198 | 396_100    | 1_536_312 | 149          | 14                            | 15                       | 26                       | 58                       |
| solve_bit_masking_v2                                                   | 581   | 3_603  | 92_305  | 309_386    | 1_291_136 | 137          | 15                            | 14                       | 27                       | 52                       |
| solve_bit_masking_v2 unchecked_indexing                                | 582   | 3_593  | 92_109  | 308_984    | 1_291_159 | 137          | 15                            | 14                       | 27                       | 53                       |
| solve_bit_masking_v2 skip_intermediate_stack_frames                    | 731   | 4_675  | 120_057 | 421_162    | 1_678_027 | 189          | 16                            | 16                       | 28                       | 53                       |
| solve_bit_masking_v2 skip_intermediate_stack_frames unchecked_indexing | 743   | 4_902  | 124_881 | 434_127    | 1_774_264 | 182          | 16                            | 16                       | 28                       | 54                       |
| solve_bitset_masking_v1                                                | 1_190 | 7_274  | 202_274 | 647_818    | 2_590_489 | 323          | 38                            | 37                       | 77                       | 149                      |
| solve_bitset_masking_v1 unchecked_indexing                             | 1_189 | 7_243  | 217_094 | 667_189    | 2_576_460 | 323          | 38                            | 37                       | 77                       | 148                      |
| solve_bit_masking_v3                                                   | 472   | 1_602  | 29_576  | 98_642     | 420_646   | 132          | 52                            | 60                       | 122                      | 256                      |
| solve_bit_masking_v3 unchecked_indexing                                | 471   | 1_564  | 30_598  | 116_990    | 444_504   | 134          | 52                            | 59                       | 111                      | 250                      |
| solve_bit_masking_v3 constraint_solve_full_sort                        | 6_179 | 6_354  | 10_452  | 10_407     | 34_056    | 134          | 55                            | 62                       | 129                      | 229                      |
| solve_bit_masking_v3 constraint_solve_full_sort unchecked_indexing     | 5_035 | 5_276  | 9_116   | 9_019      | 29_298    | 141          | 57                            | 62                       | 109                      | 232                      |
| solve_bit_masking_v3 constraint_solve_minimal_sort                     | 578   | 884    | 1_072   | 1_404      | 1_611     | 134          | 52                            | 64                       | 118                      | 219                      |
| solve_bit_masking_v3 constraint_solve_minimal_sort unchecked_indexing  | 530   | 833    | 1_083   | 1_308      | 1_468     | 134          | 54                            | 61                       | 113                      | 221                      |
| solve_bit_masking_v4                                                   | 430   | 713    | 1_051   | 1_223      | 1_404     | 128          | 37                            | 51                       | 67                       | 104                      |
| solve_bit_masking_v4 unchecked_indexing                                | 398   | 670    | 886     | 1_112      | 1_269     | 129          | 37                            | 52                       | 66                       | 103                      |
