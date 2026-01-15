{-# LANGUAGE BangPatterns #-}

import Data.Ord (comparing)
import Data.Bits (
    Bits(testBit, setBit, complement, (.&.), popCount, bit, (.|.))
  )
import Data.List (minimumBy)
import System.Environment (getArgs)
import Data.Array
import Criterion.Main

-- Type aliases for readability
type Grid = Array Int Int    -- 9x9 Sudoku grid stored as a flat list of 81 integers
type Mask = Int      -- Bitmask representing possible digits
type Pos = Int       -- Position in the grid (0..80)
type MaskV = Array Int Mask
type RowMaskV = MaskV
type ColMaskV = MaskV
type BoxMaskV = MaskV

-- Bitmask representing all digits 1..9
digitsMask :: Mask
digitsMask = 511 -- 0b111111111

-------------------------
-- Helper functions
-------------------------

-- Row index (0..8) for a given position
row :: Pos -> Int
row p = p `div` 9

-- Column index (0..8) for a given position
col :: Pos -> Int
col p = p `mod` 9

-- Block index (0..8) for a given position
block :: Pos -> Int
block p = (row p `div` 3) * 3 + col p `div` 3

-------------------------
-- Parsing
-------------------------

-- Parse a list of 9 strings (rows) into a Grid
-- Empty cells: '-' or '0', digits '1'..'9' as themselves
parseGrid :: [String] -> Maybe Grid
parseGrid ls
    | length ls /= 9 = Nothing
    | otherwise = traverse parseChar (concat ls) >>= \arr -> Just $ listArray (0,80) arr
  where
    parseChar '-' = Just 0
    parseChar '0' = Just 0
    parseChar c
        | c >= '1' && c <= '9' = Just (fromEnum c - fromEnum '0')
        | otherwise = Nothing

-------------------------
-- Initialize row/col/block masks
-------------------------

newMask :: MaskV
newMask = listArray (0,8) [0,0,0,0,0,0,0,0,0]

-- r, c, b are lists of bitmasks for rows, columns, and blocks
initMasks :: Grid -> Maybe (RowMaskV, ColMaskV, BoxMaskV)
initMasks g = go 0 (newMask) (newMask) (newMask)
  where
    go 81 r c b = Just (r, c, b)  -- Finished processing all positions
    go i r c b =
        let v = g ! i
        in if v == 0
           then go (i+1) r c b  -- Empty cell, skip
           else
               let bitv = bit (v-1)
                   ri = row i
                   ci = col i
                   bi = block i
                   idx = v-1
               -- Check for duplicates
               in if testBit (r ! ri) idx || testBit (c ! ci) idx || testBit (b ! bi) idx
                  then Nothing  -- Invalid grid: duplicate detected
                  else
                      -- Update masks
                      go (i+1)
                         (r // [(ri, (r ! ri .|. bitv))])
                         (c // [(ci, (c ! ci .|. bitv))])
                         (b // [(bi, (b ! bi .|. bitv))])

-------------------------
-- Candidate computation
-------------------------

-- Compute bitmask of possible digits for a position
candidates :: Grid -> RowMaskV -> ColMaskV -> BoxMaskV -> Pos -> Mask
candidates g r c b p
    | g ! p /= 0 = 0  -- Already filled
    | otherwise = digitsMask .&. complement ((r ! row p) .|. (c ! col p) .|. (b ! block p))

-------------------------
-- MRV (Minimum Remaining Values) selection
-------------------------

-- Select the empty cell with the fewest candidates
selectPos :: Grid -> RowMaskV -> ColMaskV -> BoxMaskV -> Maybe (Pos, Mask)
selectPos g r c b =
    let opts = [ (p, m) | p <- [0..80], g ! p == 0, let m = candidates g r c b p, m /= 0 ]
    in if null opts
       then Nothing
       else Just $ minimumBy (comparing (popCount . snd)) opts  -- Least options first

-------------------------
-- Solver
-------------------------

solve :: Grid -> RowMaskV -> ColMaskV -> BoxMaskV -> Maybe Grid
solve g r c b =
    case selectPos g r c b of
        Nothing ->
            if all (/=0) g then Just g else Nothing  -- Grid complete
        Just (p, m) -> tryVals (bits m)
          where
            tryVals [] = Nothing
            tryVals (v:vs) =
                let bitv = bit (v-1)
                    ri = row p
                    ci = col p
                    bi = block p
                    -- Update grid and masks
                    g' = g // [(p, v)]
                    r' = r // [(ri, (r ! ri .|. bitv))]
                    c' = c // [(ci, (c ! ci .|. bitv))]
                    b' = b // [(bi, (b ! bi .|. bitv))]
                in case solve g' r' c' b' of
                    Just sol -> Just sol
                    Nothing  -> tryVals vs

-- Convert bitmask to list of digits
bits :: Mask -> [Int]
bits m = [i+1 | i <- [0..8], testBit m i]

-------------------------
-- Pretty printing
-------------------------

showGrid :: Grid -> String
showGrid g =
    unlines [ unwords [ show (g ! (r*9 + c)) | c <- [0..8] ] | r <- [0..8] ]

-------------------------
-- Main
-------------------------
main :: IO ()
main = do
  let puzzle =
        [ "53--7----"
        , "6--195---"
        , "-98----6-"
        , "8---6---3"
        , "4--8-3--1"
        , "7---2---6"
        , "-6----28-"
        , "---419--5"
        , "----8--79"
        ]
  Criterion.Main.defaultMain
    [ Criterion.Main.bench "solve puzzle" $
        Criterion.Main.whnf (\ls ->
            case parseGrid ls >>= \g ->
                 initMasks g >>= \(r,c,b) -> solve g r c b of
              Nothing -> 0
              Just _  -> 1
        ) puzzle
    ]

{--main :: IO ()
main = do
    args <- getArgs
    case args of
        [fp] -> do
            ls <- lines <$> readFile fp
            case parseGrid (take 9 ls) >>= \g -> initMasks g >>= \(r,c,b) -> solve g r c b of
                Nothing  -> putStrLn "No solution"
                Just sol -> putStrLn ("Solved Sudoku:\n" ++ showGrid sol)
        _ -> putStrLn "Usage: ./solver <board_file>" --}
