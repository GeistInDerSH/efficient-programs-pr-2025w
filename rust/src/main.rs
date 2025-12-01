#[cfg(feature = "measure_runtime")]
use rand::prelude::SliceRandom;

use std::io;
use std::io::BufRead;
use std::time::{Duration, Instant};
use sudoku_solver::{Board, SudokuSolver};

#[cfg(feature = "measure_runtime")]
fn run(runtime: Duration, boards: &[sudoku_solver::Board]) -> std::io::Result<()> {
    let start = Instant::now();
    let mut end = start;

    let mut solved = 0;
    let runtime_micros = runtime.as_micros();
    while (end - start).as_micros() < runtime_micros {
        for board in boards.iter() {
            if board.solve().is_none() {
                return Err(std::io::Error::other(format!(
                    "Failed to solve board: {board:?}"
                )));
            }
        }
        solved += 1;
        end = Instant::now();
    }
    solved *= boards.len();
    let duration = (end - start).as_micros();
    let puzzles_per_second = (1_000_000 * solved) as u128 / duration;
    let usec_per_puzzle = duration / solved as u128;
    println!(
        "Solved {solved} boards in {duration} μs\n{puzzles_per_second} puzzles/s\n{usec_per_puzzle} μs/puzzle",
    );
    Ok(())
}

#[cfg(feature = "measure_runtime")]
fn from_single_file(filename: &str) -> std::io::Result<Vec<sudoku_solver::Board>> {
    let file = std::fs::File::open(filename)?;
    let buff = std::io::BufReader::new(file);

    let v = buff
        .lines()
        .map(|line| line.unwrap())
        .flat_map(|line| {
            let mut buffer = [0; 81];
            let mut index = 0;
            for i in line.as_bytes() {
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
                    b'.' => {
                        buffer[index] = 0;
                        index += 1;
                    }
                    b'\n' => {}
                    _ => {
                        return Err(io::Error::new(io::ErrorKind::InvalidData, "Invalid input"));
                    }
                }
            }
            Ok(Board::from(buffer))
        })
        .collect();

    Ok(v)
}

fn main() -> std::io::Result<()> {
    #[cfg(feature = "measure_runtime")]
    {
        let file_name = std::env::args()
            .nth(2)
            .unwrap_or("/private/tmp/tdoku/data/puzzles1_unbiased".to_string());

        let mut boards = from_single_file(&file_name)?;
        boards.shuffle(&mut rand::rng());
        run(Duration::from_secs(20), &boards)?;
    }
    #[cfg(not(feature = "measure_runtime"))]
    {
        let file_path = std::env::args().nth(1).expect("No sudoku file specified");
        let board = sudoku_solver::read_file(&file_path)?;
        let solution = board.solve();
        match solution {
            None => println!("No solution found."),
            Some(solution) => println!("Solution:\n{solution}"),
        }
    }
    Ok(())
}
