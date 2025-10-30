use crate::{Board, Solution};
use std::ops::Div;

/// Check to see if the given value can be placed at the given row and column inside the grid.
/// This requires that there are no matching values in the same row, column or "box"
fn is_valid(grid: [u8; 81], row: usize, col: usize, value: u8) -> bool {
    // is unique in row
    for v in 0..9 {
        if value == grid[row * 9 + v] {
            return false;
        }
    }

    // is unique in col
    for v in 0..9 {
        if value == grid[v * 9 + col] {
            return false;
        }
    }

    // is unique in box
    let box_start_col = col.div(3) * 3;
    let box_end_col = col.div(3) * 3 + 3;
    let box_start_row = row.div(3) * 3;
    let box_end_row = row.div(3) * 3 + 3;
    for r in box_start_row..box_end_row {
        for c in box_start_col..box_end_col {
            if value == grid[r * 9 + c] {
                return false;
            }
        }
    }

    true
}

/// Attempt to solve the sudoku grid by recursively calling [`solve`] to use backtracking.
fn solve(grid: &mut [u8; 81], row: usize, col: usize) -> bool {
    if row == 9 {
        true
    } else if col == 9 {
        solve(grid, row + 1, 0)
    } else if grid[row * 9 + col] != 0 {
        solve(grid, row, col + 1)
    } else {
        for p in 1..=9 {
            if !is_valid(*grid, row, col, p) {
                continue;
            }
            grid[row * 9 + col] = p;
            if solve(grid, row, col + 1) {
                return true;
            }
            grid[row * 9 + col] = 0;
        }

        false
    }
}

impl super::SudokuSolver for Board {
    fn solve(&self) -> Option<Solution> {
        let mut grid = self.0;

        if solve(&mut grid, 0, 0) {
            Some(grid.into())
        } else {
            None
        }
    }
}
