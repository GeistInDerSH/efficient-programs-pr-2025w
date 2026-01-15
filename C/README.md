********************* Usage *********************

---------> run "make"

---------> run "./sudoku_test" to test all the Sudoku versions with each Board from "../boards" (12 boards until now)
For each program/optimized/unoptimized program, a report will be generated, similar with this:

------> solve_optimized_v3()
Board invalid: ../boards/invalid-characters.sudoku
Board invalid: ../boards/invalid-box-collision.sudoku
Board invalid: ../boards/invalid-col-collision.sudoku
Board invalid: ../boards/invalid-row-col-collision.sudoku
Board invalid: ../boards/invalid-row-collision.sudoku
Fully Solved              : PASS
Solvable 2x hard          : PASS
Solvable Easy 1           : PASS
Solvable example 1        : PASS
Solvable extra hard 1     : PASS
Solvable hard 1           : PASS
Solvable medium 1         : PASS

Each program run for each board must print "PASS".
In total there are no_of_programs x no_of_boards program runs.
This way, the check the correctness of the implementations.

---------> run "./sudoku_bench" to see the benchmark for each of the above tests

One example

************************ Combined loops ************************ 
solve_optimized_v1()
Solver Only - Fully Solved              : 681 ns/iter
Full Run    - Fully Solved              : 6516 ns/iter
Full Run    - Invalid Characters        : 4494 ns/iter
Full Run    - Invalid Box Collision     : 5641 ns/iter
Full Run    - Invalid Col Collision     : 6538 ns/iter
Full Run    - Invalid Row Col Collision : 5652 ns/iter
Full Run    - Invalid Row Collision     : 4906 ns/iter
Solver Only - Solvable 2x hard          : 11373680 ns/iter
Full Run    - Solvable 2x hard          : 11954926 ns/iter
Solver Only - Solvable Easy 1           : 5396 ns/iter
Full Run    - Solvable Easy 1           : 9961 ns/iter
Solver Only - Solvable example 1        : 5085534 ns/iter
Full Run    - Solvable example 1        : 5032682 ns/iter
Solver Only - Solvable extra hard 1     : 2551163 ns/iter
Full Run    - Solvable extra hard 1     : 2598443 ns/iter
Solver Only - Solvable hard 1           : 797959 ns/iter
Full Run    - Solvable hard 1           : 793382 ns/iter
Solver Only - Solvable medium 1         : 30408 ns/iter
Full Run    - Solvable medium 1         : 36407 ns/iter


-----------> run "././sudoku_solver" if you want to manually test one particular implementation
with one particular board.

Usage:

george@george-PC:~/Desktop/efficient-programs-pr-2025w/C$ ./sudoku_solver 
Usage: <optimization index> <sudoku file> 
Optimization index can be: 
0 - addition of a variable 
1 - combined loops 
2 - Bitmask 
3 - cache optimization 
4 - Loop unrolling 
5 - Lookup table 
6 - unoptimized version



Example of usage:

george@george-PC:~/Desktop/efficient-programs-pr-2025w/C$ ./sudoku_solver 2 ../boards/solvable-extra-hard-1.sudoku 
optimization_index = 2 
Solution:
284196753
396745218
157832496
415378962
763429185
928561374
672984531
831257649
549613827

Took 760μs
george@george-PC:~/Desktop/efficient-programs-pr-2025w/C$ 


-----------> run "python3 benchmark.py" to measure the performance of each Sudoku version.

The output will be generated in the file benchmark_results.csv.
This python script runs each sudoku version with each board for five (5) times.

It is measuring the time, cycles and instructions.
It saves the minimum, average and maximum time for each program.
The same happens with the cycles and the instructions 

One example of the beginning of a "benchmark_results.csv" file:

program,board,wall_ns_min,wall_ns_avg,wall_ns_max,cycles_min,cycles_avg,cycles_max,instructions_min,instructions_avg,instructions_max
/home/george/Desktop/efficient-programs-pr-2025w/C/sudoku_solver 6,fully-solved.sudoku,22279986,33461934,71599939,2244014,7838358,14011740,0,0,0
/home/george/Desktop/efficient-programs-pr-2025w/C/sudoku_solver 6,invalid-box-collision.sudoku,21473167,22942215,24431400,1544548,3378215,7884088,0,0,0
/home/george/Desktop/efficient-programs-pr-2025w/C/sudoku_solver 6,invalid-col-collision.sudoku,22086386,24209699,27743068,1473564,6516551,22494670,0,0,0



-----------> run "./sudoku_bench_csv" to get the same information as from "sudoku_bench", but saved into a csv file: "benchmark_results_c.csv"

One example of the beginning of a "benchmark_results_c.csv" file:

program,opt_index,board,mode,ns_per_iter_avg
/home/ep25/ep52408125/Project/code/sudoku_bench_csv,0,fully-solved.sudoku,solver_only,2747
/home/ep25/ep52408125/Project/code/sudoku_bench_csv,0,fully-solved.sudoku,full_run,16052
/home/ep25/ep52408125/Project/code/sudoku_bench_csv,0,invalid-characters.sudoku,full_run,12284









********************* Sudoku explained *********************

The board has the size 9 by 9 - 81 cells

Rules:
    Each of the digits 1-9 must occur exactly once in each row.
    Each of the digits 1-9 must occur exactly once in each column.
    Each of the digits 1-9 must occur exactly once in each of the 9, 3x3 sub-boxes of the grid.

Zeros indicate blanks and must be filled with a number from 1 to 9.

********************* Unoptimized version *********************


The (unoptimized) and the default idea to solve Sudoku is by employing backtracking.
Here, we recursively try to fill the empty cells with numbers from 1 to 9.
For every cell that doesn't have yet a number, we place a number and then we check whether our decision was valid,
meaning to check if that number is valid according to the above rules.
If it is valid, we move to the next cell, otherwise, we backtrack and try another number.

This process/algorithm continues untill all cells are filled or no solution exists.

I wrote the function "int is_valid(const struct Board* board, int row, int col, uint8_t value)" in order to check
if I will break Sudoku rules if I put the value from the variable "value" at the row "row" and the column "col".

This algorithm has an exponential complexity

O(9^(N*N)) - where N is the size of the grid

Space complexity: O(81) = O(1)


********************* Optimized version 0 *********************

Here, in the is_valid_combined() function, this variable has been added:

int row_offset = row * 9;

so that in the first loop, when we check the row, we skip the 
computation row * 9 and we replace it with row_offset instead




********************* Optimized version 1 (Combined loops) *********************

Inside the "is_valid()" function, the first two for loops are combined.

The overhead with the initialization, verification and the counter increment is reduced.

Also, "board->cells" is stored as a pointer and used through this pointer:

const uint8_t* cells = board->cells;


********************* Optimized version 2 (Bitmask) *********************


- the bottleneck of the algorithm is the function is_valid()
- this funciton is called for each number from 1 to 9, number which the algorithm tries to place it in an empty cell
- each time the function scans 9(row) + 9(column) + 9(box) = 27 cells
- This is a waste of time, because the same scans are performed thousands of times  

Optimization stragegy: Bit masks
- removal of the is_valid() function and memorizing the table state in an efficient manner
- For this purpose, 3 arrays will provide the lookup functionality: 
    uint16_t rows[9];
    uint16_t cols[9];
    uint16_t boxes[9]; 

- Each one of these vectors is actually a bit mask of 16 bits. We actually need only 9 of them. This works as follows:
 - If bit 0 is set, it means the number 1 exists
 - If bit 1 is set, it means the number 2 exists
 - and so on

- For example, if rows[0] (the first row) contains the numbers 1, 3 and 5, then the binary value will be: 0001 0101

How does it work ?

1) Verification
 - To see if number p is valid in (row, col), the mask is 1 << (p - 1). The verification becomes:

    int box_idx = (row / 3) * 3 + (col / 3);
    uint16_t mask = 1 << (p - 1);

    if ( !(rows[row] & mask) &&  // Check if the bit is NOT set on this row
        !(cols[col] & mask) &&  // Check if the bit is NOT set on this column
        !(boxes[box_idx] & mask) ) // Check if the bit is NOT set in the box
    {
        // Valid!
    }

    By employing the mitmasks approach, the complexity of the verification drops from O(n) to O(1)

2) When setting the number p, we perform these steps:

    rows[row] |= mask;
    cols[col] |= mask;
    boxes[box_idx] |= mask;

3) Backtracking: When we go back recursively, we unset the bit:

    rows[row] &= ~mask;
    cols[col] &= ~mask;
    boxes[box_idx] &= ~mask;


The key optimization lies in avoiding repeated scanning of the row, column, and 3×3 box each time we place a number. Instead, we maintain three bitmask arrays—rows, cols, and boxes. In these arrays, each bit indicates whether a particular number has already been used in that row, column, or box.


********************* Optimized version 3 (Cache optimizations) ********************* TODO

This technique is aimed to solve one particular problem with the cache usage for the other versions.

Although, initially I thought that the changes will not have an impact, since the whole Sudoku board
contains only 81 bytes and those bytes can fit in the L1 cache memory, hence no cache misses, the 
measurements results preoved me otherwise.

In a nutshell, a normal Sudoku board is stored row-major: cells[row * 9 + col].
This implies that checking a row is cache-friendly, since 9 subsequence bytes are read.
The spacial locality in this case is perfect.
This is the fastest way to access the memory. When the CPU brings the cell "i" in cache, it also 
bringes the rest of the cache line (the next 63 bytes), so the next iterations are basically instant.


But, checking a column is not cache-friendly. First cells[col] is read, then cells[col+9], then 
cells[col + 18] and so on. The stride size is 9 and we access the "next" location with a step.
This doesn't employ the CPU caching/prefetching and is bad for the cache


The core idea is to store the same board twice:

struct Board_CacheOptimized {
    uint8_t cells_row_major[81]; // normal
    uint8_t cells_col_major[81]; // transposed (column-major)
};

So a column is also stored contiguously in memory:
- Row major index: row*9 + col
- Column major (transposed) index: col*9 + row

The core changes are here:

const uint8_t* row_data = &board->cells_row_major[row * 9];
const uint8_t* col_data = &board->cells_col_major[col * 9];

for (int v = 0; v < 9; v++) {
    if (row_data[v] == value) return 0; // sequential
    if (col_data[v] == value) return 0; // also sequential now!
}

THis means:
- Row check: sequential (same as before)
- Column check: now also sequential because the column is stored as a contiguous 9-byte block in cells_col_major


********************* Optimized version 4 (Loop unrolling) *********************

This optimization is based on the fact that the loops are not "free".
During each iteration, the CPU has to increment the counter, to compare the counter with the limit
and to execute a jump

Knowing that the loops are already executed 9 times, we can remove them and we can manually write
all the checks.

This way, the loop overhead is removed: no incrementation, no comparisons and no jumps.
This way, the CPU can execute a linear sequence of instruction faster.

The drawback: the code becomes longer and uglier. The code vizibility is sacrificed.



********************* Optimized version 5 (Lookup table) *********************

This optimization is focusing on the grid setup in the beginning

int box_start_row = (row / 3) * 3; int box_start_col = (col / 3) * 3;

The division (/) and multiplication (*) operations are on integers very slow than a simple memory(loopup) read

We replace the slow CPU instructions (DIV, MUL) with faster instructions (MOV)

This works well because the data size is small an they will fit perfectly in the L1 cache of the CPU, making the read almost instant.
     
The Lookup Table is an elegant optimization which replaces math operations with fast memory access







