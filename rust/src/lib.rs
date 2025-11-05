use std::fmt::{Debug, Display};
use std::io;
use std::io::Read;

#[cfg(feature = "solve_basic")]
pub mod basic;
#[cfg(feature = "solve_bit_masking_v1")]
pub mod bit_masking_v1;
#[cfg(feature = "solve_bit_masking_v2")]
pub mod bit_masking_v2;
#[cfg(feature = "solve_bitset_masking_v1")]
pub mod bitset_masking_v1;

/// The [`SudokuSolver`] trait is one that any solution attempt should implement on [`Board`] in
/// a submodule. That submodule should then be conditionally imported if a feature flag is set.
/// Only a single feature flag should be able to be set at one time.
/// This allows for building with
/// `cargo build --release --no-default-features --features=feature-flag` to test a single
/// improvement at a time without changing any of the setup code.
pub trait SudokuSolver {
    fn solve(&self) -> Option<Solution>;
}

pub type Solution = Board;

/// Board is a 1d array of bytes that represents a 9x9 sudoku board.
///
/// Functions have been added to allow for converting 2d arrays into the correct a board to make
/// it easier to write out data when testing.
/// ```
/// use sudoku_solver::Board;
/// let buff_1d = [
///     0, 0, 0, 0, 4, 0, 0, 0, 0,
///     0, 0, 0, 0, 0, 0, 4, 0, 9,
///     0, 0, 0, 0, 0, 5, 0, 0, 0,
///     2, 8, 0, 5, 0, 0, 3, 9, 1,
///     0, 0, 1, 3, 7, 0, 6, 0, 0,
///     0, 0, 3, 0, 8, 0, 0, 7, 0,
///     0, 2, 0, 0, 5, 0, 9, 1, 0,
///     0, 0, 5, 0, 2, 0, 0, 0, 0,
///     0, 9, 0, 6, 0, 1, 5, 0, 0,
/// ];
/// let buff_2d = [
///     [0, 0, 0, 0, 4, 0, 0, 0, 0],
///     [0, 0, 0, 0, 0, 0, 4, 0, 9],
///     [0, 0, 0, 0, 0, 5, 0, 0, 0],
///     [2, 8, 0, 5, 0, 0, 3, 9, 1],
///     [0, 0, 1, 3, 7, 0, 6, 0, 0],
///     [0, 0, 3, 0, 8, 0, 0, 7, 0],
///     [0, 2, 0, 0, 5, 0, 9, 1, 0],
///     [0, 0, 5, 0, 2, 0, 0, 0, 0],
///     [0, 9, 0, 6, 0, 1, 5, 0, 0],
/// ];
///
/// let board_1d: Board = buff_1d.into();
/// let board_2d: Board = buff_2d.into();
/// assert_eq!(board_1d, board_2d);
/// ```
#[derive(Copy, Clone, PartialEq)]
pub struct Board([u8; 81]);

impl From<[u8; 81]> for Board {
    fn from(board: [u8; 81]) -> Self {
        Board(board)
    }
}

impl From<[[u8; 9]; 9]> for Board {
    fn from(board: [[u8; 9]; 9]) -> Self {
        let mut buff = [0; 81];
        for (offset, row) in board.iter().enumerate() {
            buff[offset * 9..offset * 9 + 9].copy_from_slice(&row[..]);
        }
        Board(buff)
    }
}

impl Display for Board {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in 0..9 {
            for col in 0..9 {
                write!(f, "{}", self.0[row * 9 + col])?;
            }
            writeln!(f)?;
        }
        Ok(())
    }
}

impl Debug for Board {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{self}")
    }
}

/// Attempt to read a sudoku board from a file.
///
/// # Errors
/// * The given file does not exist
/// * The file is smaller than 89 bytes
/// * Data in the file is not digits 0 to 9
pub fn read_file(filename: &str) -> io::Result<Board> {
    let mut file = std::fs::File::open(filename)?;

    // 9 rows of 10 chars, but the last may leave out the new-line
    let mut data = [0; 89];
    file.read_exact(&mut data)?;

    // Copy bytes out of the string. Each line should be 10 bytes long, 9 digits and 1 new line.
    // Because of the new line, we need to add a small correction when addressing into the 1d array
    let mut buffer = [0; 81];
    let mut index = 0;
    for i in &data {
        match *i {
            b'0'..=b'9' => {
                buffer[index] = *i - b'0';
                index += 1;
            }
            b'\n' => {}
            _ => {
                return Err(io::Error::new(io::ErrorKind::InvalidData, "Invalid input"));
            }
        }
    }

    if index == 81 {
        Ok(buffer.into())
    } else {
        Err(io::Error::new(io::ErrorKind::InvalidData, "Invalid input"))
    }
}

/// This is a macro that evaluates an expression, i.e. the body, and returns the result
/// of the expression along with how long it took to evaluate it.
///
/// ```
/// use sudoku_solver::time_it;
/// use std::time::Instant;
///
/// let (result, time) = time_it!({
///   let a = 100;
///   a * 100_000
/// });
/// println!("{} took {}", result, time.as_nanos());
/// ```
#[macro_export]
macro_rules! time_it {
    ($x:expr) => {{
        let start = Instant::now();
        let res = $x;
        let duration = start.elapsed();
        (res, duration)
    }};
}

pub mod example_boards {
    use super::*;

    const fn board_from_2d(array: [[u8; 9]; 9]) -> Board {
        let mut buff = [0; 81];
        let mut row = 0;
        while row < 9 {
            let row_offset = row * 9;
            let mut col = 0;
            while col < 9 {
                buff[row_offset + col] = array[row][col];
                col += 1;
            }
            row += 1;
        }
        Board(buff)
    }

    pub const EASY_BOARD: Board = board_from_2d([
        [0, 5, 0, 0, 0, 9, 8, 1, 3],
        [9, 8, 0, 3, 5, 0, 6, 0, 7],
        [6, 0, 0, 0, 8, 0, 5, 9, 4],
        [8, 3, 2, 0, 0, 6, 0, 0, 0],
        [7, 0, 0, 1, 2, 8, 0, 0, 9],
        [1, 9, 0, 0, 7, 3, 2, 0, 0],
        [3, 7, 0, 6, 9, 0, 0, 5, 0],
        [0, 2, 0, 0, 0, 0, 7, 0, 0],
        [0, 0, 0, 0, 0, 4, 0, 0, 0],
    ]);
    pub const MEDIUM_BOARD: Board = board_from_2d([
        [8, 6, 0, 0, 1, 7, 0, 0, 0],
        [0, 0, 0, 0, 0, 6, 0, 3, 0],
        [5, 0, 2, 0, 9, 0, 1, 0, 0],
        [6, 9, 0, 2, 0, 4, 3, 5, 1],
        [4, 7, 5, 9, 0, 1, 8, 0, 6],
        [0, 0, 0, 8, 0, 5, 0, 7, 0],
        [7, 0, 6, 0, 0, 0, 0, 9, 3],
        [0, 0, 0, 0, 0, 9, 6, 0, 2],
        [0, 2, 0, 0, 5, 0, 0, 0, 0],
    ]);
    pub const HARD_BOARD: Board = board_from_2d([
        [0, 9, 0, 0, 1, 0, 0, 7, 2],
        [0, 0, 0, 6, 9, 0, 0, 3, 1],
        [0, 0, 3, 0, 7, 0, 5, 0, 9],
        [3, 8, 0, 4, 0, 0, 1, 0, 0],
        [0, 0, 0, 0, 8, 1, 0, 9, 0],
        [0, 0, 0, 0, 0, 0, 0, 0, 0],
        [7, 3, 0, 9, 2, 0, 0, 0, 0],
        [6, 5, 0, 0, 0, 7, 0, 0, 8],
        [0, 4, 0, 0, 0, 0, 7, 6, 0],
    ]);
    pub const EXTRA_HARD_BOARD: Board = board_from_2d([
        [0, 0, 0, 0, 0, 6, 7, 0, 0],
        [3, 9, 0, 7, 0, 0, 0, 1, 0],
        [1, 0, 0, 0, 0, 0, 4, 0, 0],
        [4, 0, 0, 0, 0, 0, 0, 6, 0],
        [7, 0, 0, 0, 2, 0, 0, 0, 0],
        [0, 0, 8, 5, 0, 0, 3, 0, 0],
        [6, 0, 0, 9, 0, 0, 5, 0, 0],
        [8, 3, 0, 0, 5, 0, 6, 4, 0],
        [0, 4, 9, 6, 1, 3, 0, 2, 7],
    ]);
    pub const HARD_2X_BOARD: Board = board_from_2d([
        [0, 7, 3, 0, 0, 2, 0, 0, 0],
        [0, 0, 0, 0, 7, 0, 0, 0, 0],
        [0, 0, 1, 0, 3, 0, 8, 0, 0],
        [0, 0, 0, 2, 6, 0, 0, 8, 0],
        [0, 0, 0, 5, 0, 0, 0, 2, 0],
        [0, 0, 0, 0, 0, 0, 1, 3, 7],
        [6, 0, 0, 4, 0, 9, 0, 0, 0],
        [0, 2, 0, 0, 0, 0, 9, 0, 0],
        [8, 0, 0, 0, 0, 0, 0, 0, 1],
    ]);
    pub const INVALID_BOARD_ROW_AND_COL_COLLISION: Board = board_from_2d([
        [0, 5, 0, 0, 0, 9, 8, 1, 3],
        [9, 8, 0, 3, 5, 0, 5, 0, 7],
        [6, 0, 0, 0, 8, 6, 0, 9, 4],
        [8, 3, 2, 0, 0, 6, 0, 0, 0],
        [7, 0, 0, 1, 2, 8, 0, 0, 9],
        [1, 9, 0, 0, 7, 3, 2, 0, 0],
        [3, 7, 0, 6, 9, 0, 0, 5, 0],
        [0, 2, 0, 0, 0, 0, 7, 0, 0],
        [0, 0, 0, 0, 0, 4, 0, 0, 0],
    ]);
    pub const INVALID_BOARD_ROW_COLLISION: Board = board_from_2d([
        [0, 5, 0, 0, 0, 9, 8, 1, 3],
        [9, 8, 0, 3, 5, 0, 5, 0, 7],
        [6, 0, 0, 0, 8, 0, 6, 9, 4],
        [8, 3, 2, 0, 0, 6, 0, 0, 0],
        [7, 0, 0, 1, 2, 8, 0, 0, 9],
        [1, 9, 0, 0, 7, 3, 2, 0, 0],
        [3, 7, 0, 6, 9, 0, 0, 5, 0],
        [0, 2, 0, 0, 0, 0, 7, 0, 0],
        [0, 0, 0, 0, 0, 4, 0, 0, 0],
    ]);
    pub const INVALID_BOARD_COL_COLLISION: Board = board_from_2d([
        [0, 5, 0, 0, 0, 9, 8, 1, 3],
        [9, 8, 0, 3, 5, 6, 0, 0, 7],
        [6, 0, 0, 0, 8, 0, 5, 9, 4],
        [8, 3, 2, 0, 0, 6, 0, 0, 0],
        [7, 0, 0, 1, 2, 8, 0, 0, 9],
        [1, 9, 0, 0, 7, 3, 2, 0, 0],
        [3, 7, 0, 6, 9, 0, 0, 5, 0],
        [0, 2, 0, 0, 0, 0, 7, 0, 0],
        [0, 0, 0, 0, 0, 4, 0, 0, 0],
    ]);
    pub const INVALID_BOARD_BOX_COLLISION: Board = board_from_2d([
        [0, 5, 0, 0, 0, 9, 8, 1, 3],
        [9, 8, 0, 3, 5, 0, 6, 0, 7],
        [6, 0, 0, 0, 8, 0, 5, 9, 4],
        [8, 3, 2, 0, 0, 6, 0, 0, 0],
        [7, 0, 0, 1, 2, 8, 0, 0, 9],
        [1, 9, 0, 0, 7, 3, 2, 0, 0],
        [3, 7, 0, 6, 9, 0, 0, 5, 0],
        [0, 2, 0, 0, 0, 0, 7, 0, 0],
        [0, 0, 3, 0, 0, 4, 0, 0, 0],
    ]);
    pub const SOLVED_BOARD: Board = board_from_2d([
        [1, 3, 9, 2, 4, 6, 7, 5, 8],
        [5, 7, 2, 8, 1, 3, 4, 6, 9],
        [4, 6, 8, 7, 9, 5, 1, 3, 2],
        [2, 8, 7, 5, 6, 4, 3, 9, 1],
        [9, 4, 1, 3, 7, 2, 6, 8, 5],
        [6, 5, 3, 1, 8, 9, 2, 7, 4],
        [7, 2, 6, 4, 5, 8, 9, 1, 3],
        [3, 1, 5, 9, 2, 7, 8, 4, 6],
        [8, 9, 4, 6, 3, 1, 5, 2, 7],
    ]);
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::SudokuSolver;

    #[test]
    fn test_is_valid() {
        let board = example_boards::SOLVED_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
        let solution = solution.unwrap();
        let expected = example_boards::SOLVED_BOARD;
        assert_eq!(solution, expected);
    }

    #[test]
    fn solve_easy() {
        let board = example_boards::EASY_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
    }

    #[test]
    fn solve_medium() {
        let board = example_boards::MEDIUM_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
    }

    #[test]
    fn solve_hard() {
        let board = example_boards::HARD_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
    }

    #[test]
    fn solve_extra_hard() {
        let board = example_boards::EXTRA_HARD_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
    }

    #[test]
    fn solve_2x_hard() {
        let board = example_boards::HARD_2X_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
    }

    #[test]
    fn solve_invalid_row() {
        let board = example_boards::INVALID_BOARD_ROW_COLLISION;
        let solution = board.solve();
        assert!(solution.is_none());
    }

    #[test]
    fn solve_invalid_col() {
        let board = example_boards::INVALID_BOARD_COL_COLLISION;
        let solution = board.solve();
        assert!(solution.is_none());
    }

    #[test]
    fn solve_invalid_row_and_col() {
        let board = example_boards::INVALID_BOARD_ROW_AND_COL_COLLISION;
        let solution = board.solve();
        assert!(solution.is_none());
    }

    #[test]
    fn solve_invalid_box() {
        let board = example_boards::INVALID_BOARD_BOX_COLLISION;
        let solution = board.solve();
        assert!(solution.is_none());
    }
}
