use std::fmt::{Debug, Display};
use std::io::Read;

#[cfg(feature = "basic")]
pub mod basic;

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
pub fn read_file(filename: &str) -> std::io::Result<Board> {
    let mut file = std::fs::File::open(filename)?;

    // 9 rows of 10 chars, but the last may leave out the new-line
    let mut data = [0; 89];
    file.read_exact(&mut data)?;

    // Copy bytes out of the string. Each line should be 10 bytes long, 9 digits and 1 new line.
    // Because of the new line, we need to add a small correction when addressing into the 1d array
    let mut buffer = [0; 81];
    for i in 0..9 {
        buffer[i * 9..i * 9 + 9].copy_from_slice(&data[i * 9 + i..i * 9 + 9 + i]);
    }
    // subtract ascii 0 to get digits in range 0..9
    for i in &mut buffer {
        if !(b'0' <= *i && *i <= b'9') {
            return Err(std::io::Error::new(
                std::io::ErrorKind::InvalidData,
                "Invalid input",
            ));
        }
        *i -= b'0';
    }

    Ok(buffer.into())
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
