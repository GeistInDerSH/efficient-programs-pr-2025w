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

/// All bits for the value have been seen.
///
/// Unlike in [`bit_masking_v3`] this does not set the lowest bit, and instead sets an
/// additional one higher. This lets us avoid needing to do `1 << (value - 1)` when
/// validating the board.
const ALL_BITS_SET: u16 = 0b11_1111_1110;

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

/// Model the possible states of parsing/validating the board.
enum Status {
    Solved,
    Unsolvable,
    Unsolved(Solver),
}

impl From<&Board> for Status {
    fn from(board: &Board) -> Status {
        let mut rows = [ALL_BITS_SET; 9];
        let mut cols = [ALL_BITS_SET; 9];
        let mut boxes = [ALL_BITS_SET; 9];
        let mut todo = Vec::with_capacity(81);

        for row in 0..9 {
            for col in 0..9 {
                let box_number = (row / 3) * 3 + (col / 3);
                let value = board[row * 9 + col];
                if value == 0 {
                    todo.push((row, col, box_number));
                } else {
                    let mask: u16 = 1 << value;

                    // Because we are XORing the bits, if any of them are set to 0 then
                    // we have already seen it, and should bail because the board is invalid
                    if rows[row] & cols[col] & boxes[box_number] & mask == 0 {
                        return Status::Unsolvable;
                    }

                    rows[row] ^= mask;
                    cols[col] ^= mask;
                    boxes[box_number] ^= mask;
                }
            }
        }

        if todo.is_empty() {
            Status::Solved
        } else {
            Status::Unsolved(Solver {
                board: *board,
                rows,
                cols,
                boxes,
                todo,
            })
        }
    }
}

/// Get the lowest set bit of a [`u16`]
#[inline(always)]
fn get_low_bit(value: u16) -> u16 {
    value & value.wrapping_neg()
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
    pos.trailing_zeros() as u8
}

/// Count the number of 1s in the given [`u16`], and return it as a [`u16`].
#[inline(always)]
fn count_ones(value: u16) -> u16 {
    #[cfg(target_arch = "x86_64")]
    // AMD64 has a popcnt instruction that can do this for us.
    // For some reason u16::count_ones(), which calls intrinsics::ctpop,
    // doesn't do this for us
    unsafe {
        std::arch::x86_64::_popcnt32(value as i32) as u16
    }
    #[cfg(not(target_arch = "x86_64"))]
    {
        value.count_ones() as u16
    }
}

impl crate::SudokuSolver for Board {
    fn solve(&self) -> Option<Solution> {
        match self.into() {
            Status::Unsolved(mut solution) => {
                if solution.solve(0) {
                    Some(solution.board)
                } else {
                    None
                }
            }
            Status::Unsolvable => None,
            Status::Solved => Some(*self),
        }
    }
}
