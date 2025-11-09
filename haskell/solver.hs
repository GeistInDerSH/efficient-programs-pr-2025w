-- Sudoku.hs
-- Sudoku solver that accepts input with '-' for blanks

import Data.List (transpose, minimumBy, (\\), intercalate)
import Data.Maybe (listToMaybe)
import Data.Ord (comparing)

type Grid = [[Int]]
type Pos  = (Int, Int)

digits :: [Int]
digits = [1..9]

-- Parse 9 lines of input with '-' for blanks
parseGrid :: [String] -> Maybe Grid
parseGrid ls
  | length ls /= 9 = Nothing
  | otherwise = traverse parseLine ls
  where
    parseLine s
      | length s /= 9 = Nothing
      | otherwise = Just $ map charToCell s
    charToCell c
      | c == '-'  = 0
      | c == '0'  = 0
      | c >= '1' && c <= '9' = read [c]
      | otherwise = 0
-- Print the grid
showGrid :: Grid -> String
showGrid g = intercalate "\n" $ map showRow g
  where
    showRow r = concatMap showCell r
    showCell 0 = "0 "
    showCell n = show n ++ " "
-- Return the value at a given position
valueAt :: Grid -> Pos -> Int
valueAt g (r,c) = g !! r !! c
-- Update the grid by one cell (This creates a new grid every time it is called)
updateGrid :: Grid -> Pos -> Int -> Grid
updateGrid g (r,c) val =
  take r g ++
  [take c (g !! r) ++ [val] ++ drop (c+1) (g !! r)] ++
  drop (r+1) g
-- Give all non-zero values in the specified row
rowVals :: Grid -> Int -> [Int]
rowVals g r = filter (/= 0) (g !! r)
-- Give all non-zero values in the specified column
colVals :: Grid -> Int -> [Int]
colVals g c = filter (/= 0) $ map (!! c) g
-- Give all non-zero values in the specified 3x3 block
blockVals :: Grid -> Pos -> [Int]
blockVals g (r,c) = filter (/= 0)
  [ g !! r' !! c'
  | r' <- [br .. (br + 2)]
  , c' <- [bc .. (bc + 2)]
  ]
  where
    br = (r `div` 3) * 3
    bc = (c `div` 3) * 3
-- If the cell is filled, do not return any candidates
-- Otherwise, return digits not used in the same row, column, or block
candidates :: Grid -> Pos -> [Int]
candidates g (r,c)
  | valueAt g (r,c) /= 0 = []
  | otherwise = digits \\ used
  where
    used = rowVals g r ++ colVals g c ++ blockVals g (r,c)
-- Find all empty cells in the grid
emptyPositions :: Grid -> [Pos]
emptyPositions g = [ (r,c) | r <- [0..8], c <- [0..8], valueAt g (r,c) == 0 ]

--For each empty position, compute its candidates.
--Then choose the one with the smallest number of options
--This is an MRV heuristic algorithm.
selectPos :: Grid -> Maybe (Pos, [Int])
selectPos g =
  case candidatesNonEmpty of
    [] -> Nothing
    xs -> Just $ minimumBy (comparing (length . snd)) xs
  where
    positions = emptyPositions g
    candidatesNonEmpty = [ (p, candidates g p) | p <- positions, not (null (candidates g p)) ]
-- If there are no empty positions left, return the grid as a solution
-- otherwise, continue
solve :: Grid -> [Grid]
solve g
  | null (emptyPositions g) = [g]  -- solved
  | otherwise =
      case selectPos g of
        Nothing -> []  -- dead end
        Just (pos, cs) -> concatMap tryVal cs
          where
            tryVal v = solve (updateGrid g pos v)

-- Return the grid if it is solved, otherwise continue
solveOnce :: Grid -> Maybe Grid
solveOnce g = listToMaybe (solve g)

main :: IO ()
main = do
  putStrLn "Enter 9 lines of 9 characters (digits or '-' or 0 for empty):"
  ls <- fmap lines getContents
  case parseGrid (take 9 ls) of
    Nothing -> putStrLn "Invalid input (need exactly 9 lines of 9 chars)."
    Just grid -> case solveOnce grid of
      Nothing -> putStrLn "No solution found."
      Just sol -> putStrLn $ "Solved Sudoku:\n" ++ showGrid sol
