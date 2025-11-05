use crate::{Board, Solution};
use std::ops::Div;

/// Attempt to pack the values seen in the rows/column/box into a
/// bitset to reduce space over the version in bit_masking_v1
struct BitSet {
    row_bits: u128,
    col_bits: u128,
    box_bits: u128,
}

impl BitSet {
    fn new() -> Self {
        Self {
            row_bits: 0,
            col_bits: 0,
            box_bits: 0,
        }
    }

    fn try_configure_from(board: &[u8; 81]) -> Option<Self> {
        let mut bit_set = BitSet::new();

        // No need to set custom logic to skip the 0th bit, as we won't ever check it
        // in the actual code
        for row in 0..9 {
            let row_offset = row * 9;
            let row_box = row.div(3) * 3;
            for col in 0..9 {
                let value = board[row_offset + col] as usize;
                if value == 0 {
                    continue;
                }

                let box_number = (row_box + col.div(3)) * 9 + value;
                let box_mask = 1 << box_number;
                if bit_set.box_and(box_mask) != 0 {
                    return None;
                }
                bit_set.box_set_or(box_mask);

                let row_mask = 1 << (row_offset + value);
                if bit_set.row_and(row_mask) != 0 {
                    return None;
                }
                bit_set.row_set_or(row_mask);

                let col_mask = 1 << (col * 9 + value);
                if bit_set.col_and(col_mask) != 0 {
                    return None;
                }
                bit_set.col_set_or(col_mask);
            }
        }
        Some(bit_set)
    }

    fn row_set_or(&mut self, mask: u128) {
        self.row_bits |= mask;
    }

    fn row_set_and(&mut self, mask: u128) {
        self.row_bits &= mask;
    }

    fn row_and(&mut self, mask: u128) -> u128 {
        self.row_bits & mask
    }

    fn col_set_or(&mut self, mask: u128) {
        self.col_bits |= mask;
    }

    fn col_set_and(&mut self, mask: u128) {
        self.col_bits &= mask;
    }

    fn col_and(&mut self, mask: u128) -> u128 {
        self.col_bits & mask
    }

    fn box_set_or(&mut self, mask: u128) {
        self.box_bits |= mask;
    }

    fn box_set_and(&mut self, mask: u128) {
        self.box_bits &= mask;
    }

    fn box_and(&mut self, mask: u128) -> u128 {
        self.box_bits & mask
    }
}

/// Attempt to solve the sudoku grid by recursively calling [`solve`] to use backtracking.
/// Additionally, track which bits have already been set in the grid by using three
/// arrays of 9 u16s. This allows us to set the nth bit to indicate that the number has
/// already been set in the row/column/box.
fn solve(solution_grid: &mut [u8; 81], bit_set: &mut BitSet, row: usize, col: usize) -> bool {
    if row == 9 {
        return true;
    }

    if col == 9 {
        return solve(solution_grid, bit_set, row + 1, 0);
    }

    let offset = row * 9 + col;
    if solution_grid[offset] != 0 {
        solve(solution_grid, bit_set, row, col + 1)
    } else {
        let row_offset = row * 9;
        let col_offset = col * 9;
        let box_number = (row.div(3) * 3 + col.div(3)) * 9;
        for p in 1..=9 {
            let value = p as usize;
            let box_mask = 1 << (box_number + value);
            if bit_set.box_and(box_mask) != 0 {
                continue;
            }
            let row_mask = 1 << (row_offset + value);
            if bit_set.row_and(row_mask) != 0 {
                continue;
            }
            let col_mask = 1 << (col_offset + value);
            if bit_set.col_and(col_mask) != 0 {
                continue;
            }

            // Set the values in the grid, and bitmask fields
            solution_grid[offset] = p;
            bit_set.row_set_or(row_mask);
            bit_set.col_set_or(col_mask);
            bit_set.box_set_or(box_mask);

            if solve(solution_grid, bit_set, row, col + 1) {
                return true;
            }

            // Not a solution, so rollback the above masking
            solution_grid[offset] = 0;
            bit_set.row_set_and(!row_mask);
            bit_set.col_set_and(!col_mask);
            bit_set.box_set_and(!box_mask);
        }

        false
    }
}

impl super::SudokuSolver for Board {
    fn solve(&self) -> Option<Solution> {
        let mut bit_set = BitSet::try_configure_from(&self.0)?;
        let mut grid = self.0;
        if solve(&mut grid, &mut bit_set, 0, 0) {
            Some(grid.into())
        } else {
            None
        }
    }
}
