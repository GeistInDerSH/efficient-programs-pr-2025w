#[cfg(not(feature = "unchecked_indexing"))]
mod safe_solution;
#[cfg(feature = "unchecked_indexing")]
mod unsafe_solution;

use crate::{Board, Solution};

#[cfg(all(
    feature = "constraint_solve_full_sort",
    feature = "constraint_solve_minimal_sort"
))]
compile_error!(
    "The 'constraint_solve_full_sort' and 'constraint_solve_minimal_sort' cannot be used together"
);

/// All bits for the value have been seen
const ALL_BITS_SET: u16 = 0b1_1111_1111;

type RowEntry = usize;
type ColEntry = usize;
type BoxEntry = usize;
type TodoEntry = (RowEntry, ColEntry, BoxEntry);

struct Solver {
    board: Board,
    rows: [u16; 9],
    cols: [u16; 9],
    boxes: [u16; 9],
    todo: Vec<TodoEntry>,
}

impl Default for Solver {
    fn default() -> Self {
        Self {
            board: Board([0; 81]),
            rows: [ALL_BITS_SET; 9],
            cols: [ALL_BITS_SET; 9],
            boxes: [ALL_BITS_SET; 9],
            todo: Vec::new(),
        }
    }
}

impl TryFrom<Board> for Solver {
    type Error = &'static str;

    fn try_from(board: Board) -> Result<Self, Self::Error> {
        let mut solver = Solver {
            board,
            ..Default::default()
        };

        for row in 0..9 {
            for col in 0..9 {
                let box_number = (row / 3) * 3 + (col / 3);
                let value = board[(row, col)];
                if value == 0 {
                    solver.todo.push((row, col, box_number));
                } else {
                    let mask: u16 = 1 << (value - 1);

                    // Because we are XORing the bits, if any of them are set to 0 then
                    // we have already seen it, and should bail because the board is invalid
                    if solver.rows[row] & solver.cols[col] & solver.boxes[box_number] & mask == 0 {
                        return Err("Invalid board");
                    }

                    solver.rows[row] ^= mask;
                    solver.cols[col] ^= mask;
                    solver.boxes[box_number] ^= mask;
                }
            }
        }

        Ok(solver)
    }
}

/// Get the lowest set bit of a [`u16`]
#[inline(always)]
fn get_low_bit(value: u16) -> u16 {
    value & (0 - value)
}

/// Invert the lowest bit of a [`u16`]
#[inline(always)]
fn clear_low_bit(value: u16) -> u16 {
    value & (value - 1)
}

/// Convert a [`u16`] with a single bit set into a [`u8`] value
/// representing the index of the bit set.
#[inline(always)]
fn pos_to_value(pos: u16) -> u8 {
    (pos.trailing_zeros() + 1) as u8
}

impl crate::SudokuSolver for Board {
    fn solve(&self) -> Option<Solution> {
        let mut solver = Solver::try_from(*self).ok()?;
        if solver.is_solved() {
            Some(*self)
        } else if solver.solve(0) {
            Some(solver.board)
        } else {
            None
        }
    }
}
