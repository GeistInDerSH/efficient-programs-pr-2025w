use sudoku_solver::SudokuSolver;

fn main() -> std::io::Result<()> {
    #[cfg(feature = "measure_runtime")]
    {
        use std::time::Instant;
        let times: usize = std::env::args()
            .nth(2)
            .unwrap_or("1".to_string())
            .parse()
            .unwrap_or(1);

        let (solution, duration) = sudoku_solver::time_it!({
            let file_path = std::env::args().nth(1).expect("No sudoku file specified");
            let mut solution = None;
            for _ in 0..times {
                let board = sudoku_solver::read_file(&file_path)?;
                solution = board.solve();
            }
            solution
        });
        let time_micros = duration.as_micros() / times as u128;
        match solution {
            None => println!("No solution found. Took {time_micros}μs"),
            Some(solution) => println!("Solution:\n{solution}\nTook {time_micros}μs"),
        }
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
