use sudoku_solver::SudokuSolver;

fn main() -> std::io::Result<()> {
    #[cfg(feature = "measure_runtime")]
    {
        use std::time::Instant;
        let (solution, duration) = sudoku_solver::time_it!({
            let file_path = std::env::args().nth(1).expect("No sudoku file specified");
            let board = sudoku_solver::read_file(&file_path)?;
            board.solve()
        });
        let time_micros = duration.as_micros();
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
