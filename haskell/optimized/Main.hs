import Solver
import System.Environment (getArgs)

main :: IO ()
main = do
  args <- getArgs
  case args of
    [fp] -> do
      ls <- lines <$> readFile fp
      case parseGrid (take 9 ls) >>= \g ->
           initMasks g >>= \(r,c,b) -> solve g r c b of
        Nothing  -> putStrLn "No solution"
        Just sol -> putStrLn ("Solved Sudoku:\n" ++ showGrid sol)
    _ -> putStrLn "Usage: ./solver <board_file>"
