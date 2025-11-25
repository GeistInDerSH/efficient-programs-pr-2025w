use std::fmt::{Debug, Display, Write};
use std::io;
#[allow(unused)]
use std::io::Read;
use std::ops::{Index, IndexMut};

#[cfg(feature = "solve_basic")]
pub mod basic;
#[cfg(feature = "solve_basic_std_index")]
pub mod basic_std_index;
#[cfg(feature = "solve_bit_masking_v1")]
pub mod bit_masking_v1;
#[cfg(feature = "solve_bit_masking_v2")]
pub mod bit_masking_v2;
#[cfg(feature = "solve_bit_masking_v3")]
pub mod bit_masking_v3;
#[cfg(feature = "solve_bit_masking_v4")]
pub mod bit_masking_v4;
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
        for (i, v) in self.0.iter().enumerate() {
            if matches!(i, 9 | 18 | 27 | 36 | 45 | 54 | 63 | 72 | 81) {
                f.write_char('\n')?;
            }
            f.write_char((*v + b'0') as char)?;
        }
        f.write_char('\n')?;
        Ok(())
    }
}

/// Add support for (row, col) indexing to make it easier to work with the Board
impl Index<(usize, usize)> for Board {
    type Output = u8;

    fn index(&self, index: (usize, usize)) -> &Self::Output {
        #[cfg(not(feature = "unchecked_indexing"))]
        {
            &self.0[index.0 * 9 + index.1]
        }
        #[cfg(feature = "unchecked_indexing")]
        unsafe {
            self.0.get_unchecked(index.0 * 9 + index.1)
        }
    }
}

impl Index<usize> for Board {
    type Output = u8;

    fn index(&self, index: usize) -> &Self::Output {
        #[cfg(not(feature = "unchecked_indexing"))]
        {
            &self.0[index]
        }
        #[cfg(feature = "unchecked_indexing")]
        unsafe {
            self.0.get_unchecked(index)
        }
    }
}

/// Add support for (row, col) indexing to make it easier to work with the Board, and
/// mutating the value at the index.
impl IndexMut<(usize, usize)> for Board {
    fn index_mut(&mut self, index: (usize, usize)) -> &mut Self::Output {
        #[cfg(not(feature = "unchecked_indexing"))]
        {
            &mut self.0[index.0 * 9 + index.1]
        }
        #[cfg(feature = "unchecked_indexing")]
        unsafe {
            self.0.get_unchecked_mut(index.0 * 9 + index.1)
        }
    }
}

impl IndexMut<usize> for Board {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        #[cfg(not(feature = "unchecked_indexing"))]
        {
            &mut self.0[index]
        }
        #[cfg(feature = "unchecked_indexing")]
        unsafe {
            self.0.get_unchecked_mut(index)
        }
    }
}

impl Debug for Board {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{self}")
    }
}

/// A quick shortcut to allow for "raw" x64 syscalls to be made on Linux.
/// This is controlled via the `raw_syscalls` feature flag. Because we only
/// use it for Open/Read/Close (in that order).
///
/// This was only made because using `cargo flamegraph` showed that when there
/// are lots of runs, the main function spends a lot of time in the Rust
/// fs::open function. This is likely because of some ffi reasons of converting
/// between Rust strings and C-Strings.
#[allow(unused)]
#[inline(always)]
#[cfg(all(feature = "raw_syscalls", target_arch = "x86_64", target_os = "linux"))]
fn x64_linux_syscall(syscall_id: u64, rdi: u64, rsi: u64, rdx: u64) -> i64 {
    let ret: i64;
    unsafe {
        std::arch::asm!(
        "syscall",
        in("rax") syscall_id,
        in("rdi") rdi,
        in("rsi") rsi,
        in("rdx") rdx,
        lateout("rax") ret,
        options(nostack)
        );
    }
    ret
}

#[allow(unused)]
#[inline(always)]
#[cfg(all(feature = "raw_syscalls", target_arch = "x86_64", target_os = "linux"))]
fn read_to_buffer(filename: &str) -> io::Result<[u8; 89]> {
    // Syscall ID and expected register value information pulled from:
    // https://blog.rchapman.org/posts/Linux_System_Call_Table_for_x86_64/
    // This info is also in the Linux Kernel, but the above table include the
    // registers, and what the contents should be.
    const SYSCALL_READ: u64 = 0;
    const SYSCALL_OPEN: u64 = 2;
    const SYSCALL_CLOSE: u64 = 3;

    let c_file_path = std::ffi::CString::new(filename)?;
    let path_ptr = c_file_path.as_ptr() as u64;
    let fd: i64 = x64_linux_syscall(SYSCALL_OPEN, path_ptr, 0, 0);
    if fd < 0 {
        return Err(io::Error::other("Failed to open file"));
    }
    let fd = fd as u64;

    let mut data = [0u8; 89];
    let bytes_read = x64_linux_syscall(
        SYSCALL_READ,
        fd,
        data.as_mut_ptr() as u64,
        data.len() as u64,
    );
    if bytes_read == !0 {
        return Err(io::Error::other("Failed to read file"));
    }

    x64_linux_syscall(SYSCALL_CLOSE, fd, 0, 0);

    Ok(data)
}

/// Attempt to read a sudoku board from a file.
///
/// # Errors
/// * The given file does not exist
/// * The file is smaller than 89 bytes
/// * Data in the file is not digits 0 to 9
pub fn read_file(filename: &str) -> io::Result<Board> {
    #[cfg(not(all(feature = "raw_syscalls", target_os = "linux", target_arch = "x86_64")))]
    let data = {
        let mut file = std::fs::File::open(filename)?;

        // 9 rows of 10 chars, but the last may leave out the new-line
        let mut data = [0; 89];
        let _ = file.read(&mut data)?;
        data
    };
    #[cfg(all(feature = "raw_syscalls", target_arch = "x86_64", target_os = "linux"))]
    let data = read_to_buffer(filename)?;

    // Copy bytes out of the string. Each line should be 10 bytes long, 9 digits and 1 new line.
    // Because of the new line, we need to add a small correction when addressing into the 1d array
    let mut buffer = [0; 81];
    let mut index = 0;
    for i in &data {
        match *i {
            b'0'..=b'9' => {
                #[cfg(not(feature = "unchecked_indexing"))]
                {
                    buffer[index] = *i - b'0';
                }
                #[cfg(feature = "unchecked_indexing")]
                unsafe {
                    *buffer.get_unchecked_mut(index) = *i - b'0';
                }
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
    use super::Board;

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
    use std::collections::HashSet;

    fn is_valid_solution(solved_board: &Solution) -> bool {
        let mut rows = vec![HashSet::new(); 9];
        let mut cols = vec![HashSet::new(); 9];
        let mut boxes = vec![HashSet::new(); 9];

        for row in 0..9 {
            for col in 0..9 {
                let value = solved_board[(row, col)];
                let box_number = (row / 3) * 3 + (col / 3);
                if !rows[row].insert(value)
                    || !cols[col].insert(value)
                    || !boxes[box_number].insert(value)
                {
                    return false;
                }
            }
        }

        let expected = HashSet::from([1, 2, 3, 4, 5, 6, 7, 8, 9]);
        for i in 0..9 {
            if expected != rows[i] || expected != cols[i] || expected != boxes[i] {
                return false;
            }
        }

        true
    }

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
        assert!(is_valid_solution(&solution.unwrap()));
    }

    #[test]
    fn solve_medium() {
        let board = example_boards::MEDIUM_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
        assert!(is_valid_solution(&solution.unwrap()));
    }

    #[test]
    fn solve_hard() {
        let board = example_boards::HARD_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
        assert!(is_valid_solution(&solution.unwrap()));
    }

    #[test]
    fn solve_extra_hard() {
        let board = example_boards::EXTRA_HARD_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
        assert!(is_valid_solution(&solution.unwrap()));
    }

    #[test]
    fn solve_2x_hard() {
        let board = example_boards::HARD_2X_BOARD;
        let solution = board.solve();
        assert!(solution.is_some());
        assert!(is_valid_solution(&solution.unwrap()));
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

    #[test]
    fn solved_board_to_string() {
        let board = example_boards::SOLVED_BOARD.to_string();
        let expected = "139246758\n572813469\n468795132\n287564391\n941372685\n653189274\n726458913\n315927846\n894631527\n";
        assert_eq!(expected, board);
    }
}
