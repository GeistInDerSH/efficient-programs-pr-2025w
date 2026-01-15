{- cabal:
build-depends: base, criterion
ghc-options: -O2 -pgml=/bin/gcc -threaded -rtsopts=all
-}

{-
This file is a single Haskell file that can be run via Cabal to build
and benchmark a solver.
Currently, it only benchmarks how long it takes to solve known good boards.
The boards match the ones in the 'boards' directory in the root of the project.

How to run:
PATH="/usr/bin/ld:$PATH" cabal run BenchmarkSolver.hs

The path prefix is needed because something is messed up with GHC on
the machine for the linker. Otherwise it looks for GNU Gold, but
that isn't resolvable.
-}

import Criterion.Main

import System.Environment (getArgs)
import Data.List (transpose, minimumBy, (\\), intercalate)
import Data.Maybe (listToMaybe, fromJust)
import Data.Ord (comparing)

type Grid = [[Int]]
type Pos  = (Int, Int)

digits :: [Int]
digits = [1..9]

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

fully_solved :: Grid
fully_solved =  [[1, 3, 9, 2, 4, 6, 7, 5, 8]
                ,[5, 7, 2, 8, 1, 3, 4, 6, 9]
                ,[4, 6, 8, 7, 9, 5, 1, 3, 2]
                ,[2, 8, 7, 5, 6, 4, 3, 9, 1]
                ,[9, 4, 1, 3, 7, 2, 6, 8, 5]
                ,[6, 5, 3, 1, 8, 9, 2, 7, 4]
                ,[7, 2, 6, 4, 5, 8, 9, 1, 3]
                ,[3, 1, 5, 9, 2, 7, 8, 4, 6]
                ,[8, 9, 4, 6, 3, 1, 5, 2, 7]]

easy :: Grid
easy = [[0, 5, 0, 0, 0, 9, 8, 1, 3]
       ,[9, 8, 0, 3, 5, 0, 6, 0, 7]
       ,[6, 0, 0, 0, 8, 0, 5, 9, 4]
       ,[8, 3, 2, 0, 0, 6, 0, 0, 0]
       ,[7, 0, 0, 1, 2, 8, 0, 0, 9]
       ,[1, 9, 0, 0, 7, 3, 2, 0, 0]
       ,[3, 7, 0, 6, 9, 0, 0, 5, 0]
       ,[0, 2, 0, 0, 0, 0, 7, 0, 0]
       ,[0, 0, 0, 0, 0, 4, 0, 0, 0]]

medium :: Grid
medium = [[8, 6, 0, 0, 1, 7, 0, 0, 0]
         ,[0, 0, 0, 0, 0, 6, 0, 3, 0]
         ,[5, 0, 2, 0, 9, 0, 1, 0, 0]
         ,[6, 9, 0, 2, 0, 4, 3, 5, 1]
         ,[4, 7, 5, 9, 0, 1, 8, 0, 6]
         ,[0, 0, 0, 8, 0, 5, 0, 7, 0]
         ,[7, 0, 6, 0, 0, 0, 0, 9, 3]
         ,[0, 0, 0, 0, 0, 9, 6, 0, 2]
         ,[0, 2, 0, 0, 5, 0, 0, 0, 0]]

hard :: Grid
hard = [[0, 9, 0, 0, 1, 0, 0, 7, 2]
       ,[0, 0, 0, 6, 9, 0, 0, 3, 1]
       ,[0, 0, 3, 0, 7, 0, 5, 0, 9]
       ,[3, 8, 0, 4, 0, 0, 1, 0, 0]
       ,[0, 0, 0, 0, 8, 1, 0, 9, 0]
       ,[0, 0, 0, 0, 0, 0, 0, 0, 0]
       ,[7, 3, 0, 9, 2, 0, 0, 0, 0]
       ,[6, 5, 0, 0, 0, 7, 0, 0, 8]
       ,[0, 4, 0, 0, 0, 0, 7, 6, 0]]

extra_hard :: Grid
extra_hard = [[0, 0, 0, 0, 0, 6, 7, 0, 0]
             ,[3, 9, 0, 7, 0, 0, 0, 1, 0]
             ,[1, 0, 0, 0, 0, 0, 4, 0, 0]
             ,[4, 0, 0, 0, 0, 0, 0, 6, 0]
             ,[7, 0, 0, 0, 2, 0, 0, 0, 0]
             ,[0, 0, 8, 5, 0, 0, 3, 0, 0]
             ,[6, 0, 0, 9, 0, 0, 5, 0, 0]
             ,[8, 3, 0, 0, 5, 0, 6, 4, 0]
             ,[0, 4, 9, 6, 1, 3, 0, 2, 7]]

hard_2x :: Grid
hard_2x = [[0, 7, 3, 0, 0, 2, 0, 0, 0]
          ,[0, 0, 0, 0, 7, 0, 0, 0, 0]
          ,[0, 0, 1, 0, 3, 0, 8, 0, 0]
          ,[0, 0, 0, 2, 6, 0, 0, 8, 0]
          ,[0, 0, 0, 5, 0, 0, 0, 2, 0]
          ,[0, 0, 0, 0, 0, 0, 1, 3, 7]
          ,[6, 0, 0, 4, 0, 9, 0, 0, 0]
          ,[0, 2, 0, 0, 0, 0, 9, 0, 0]
          ,[8, 0, 0, 0, 0, 0, 0, 0, 1]]

main = defaultMain [
	 bgroup "solved" [bench "full solved"  $ whnf solveOnce fully_solved
                     ,bench "easy"         $ whnf solveOnce easy
			         ,bench "medium"       $ whnf solveOnce  medium
			         ,bench "hard"         $ whnf solveOnce  hard
			         ,bench "extra hard"   $ whnf solveOnce  extra_hard
			         ,bench "2x hard"      $ whnf solveOnce  hard_2x
			 ]
	]
