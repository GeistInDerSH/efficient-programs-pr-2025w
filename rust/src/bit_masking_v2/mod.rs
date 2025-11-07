/// Similar to bit_masking_v1, but delays the compares by bitwise-or'ing
/// the values then comparing the result all at once. This avoids extra
/// compare/jump instructions.
use crate::{Board, Solution};
use std::ops::Div;


/// Attempt to solve the sudoku grid by recursively calling [`solve`] to use backtracking.
/// Additionally, track which bits have already been set in the grid by using three
/// arrays of 9 u16s. This allows us to set the nth bit to indicate that the number has
/// already been set in the row/column/box.
fn solve(
    solution_grid: &mut Board,
    row_bits: &mut [u16; 9],
    col_bits: &mut [u16; 9],
    box_bits: &mut [u16; 9],
    row: usize,
    col: usize,
) -> bool {
    if row == 9 {
        true
    } else if col == 9 {
        solve(solution_grid, row_bits, col_bits, box_bits, row + 1, 0)
    } else if solution_grid[(row, col)] != 0 {
        solve(solution_grid, row_bits, col_bits, box_bits, row, col + 1)
    } else {
        let box_number = row.div(3) * 3 + col.div(3);
        for p in 1..=9 {
            let mask = 1 << p;

            let any_seen = {
                let mut seen = 0;
                seen |= row_bits[row] & mask;
                seen |= col_bits[col] & mask;
                seen |= box_bits[box_number] & mask;
                seen
            };
            if any_seen != 0 {
                continue;
            }

            // Set the values in the grid, and bitmask fields
            solution_grid[(row, col)] = p;
            row_bits[row] |= mask;
            col_bits[col] |= mask;
            box_bits[box_number] |= mask;

            if solve(solution_grid, row_bits, col_bits, box_bits, row, col + 1) {
                return true;
            }

            // Not a solution, so rollback the above masking
            solution_grid[(row, col)] = 0;
            row_bits[row] &= !mask;
            col_bits[col] &= !mask;
            box_bits[box_number] &= !mask;
        }

        false
    }
}

impl super::SudokuSolver for Board {
    fn solve(&self) -> Option<Solution> {
        let mut grid = Board(self.0);
        let mut rows = [0; 9];
        let mut cols = [0; 9];
        let mut boxes = [0; 9];

        // No need to set custom logic to skip the 0th bit, as we won't ever check it
        // in the actual code
        for row in 0..9 {
            for col in 0..9 {
                let value = grid[(row, col)];
                if value == 0 {
                    continue;
                }

                let box_number = row.div(3) * 3 + col.div(3);
                let mask = 1 << value;
                let any_seen = {
                    let mut seen = 0;
                    seen |= rows[row] & mask;
                    seen |= cols[col] & mask;
                    seen |= boxes[box_number] & mask;
                    seen
                };
                if any_seen != 0 {
                    return None;
                }

                rows[row] |= mask;
                cols[col] |= mask;
                boxes[box_number] |= mask;
            }
        }

        if solve(&mut grid, &mut rows, &mut cols, &mut boxes, 0, 0) {
            Some(grid)
        } else {
            None
        }
    }
}
