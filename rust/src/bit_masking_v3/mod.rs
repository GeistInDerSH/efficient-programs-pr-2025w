use crate::{Board, Solution};

/// All bits for the value have been seen
const ALL_BITS_SET: u16 = 0b1_1111_1111;

type RowEntry = usize;
type ColEntry = usize;
type BoxEntry = usize;
type TodoEntry = (RowEntry, ColEntry, BoxEntry);

struct Solver {
    board: Board,
    rows: [u16; 9],
    cols: [u16; 9],
    boxes: [u16; 9],
    todo: Vec<TodoEntry>,
}

impl Solver {
    #[cfg(not(feature = "unchecked_indexing"))]
    fn solve(&mut self, index: usize) -> bool {
        // Sort so the cell with the least number of candidates is first. The idea
        // being we can solve the easy ones first, then it helps to eliminate options
        // for cells with more possibilities
        #[cfg(feature = "priority_queue")]
        self.todo[index..].sort_by(|a, b| {
            let a_count = {
                let (row, col, box_num) = a;
                let candidates = self.rows[*row] & self.cols[*col] & self.boxes[*box_num];
                candidates.count_ones()
            };
            let b_count = {
                let (row, col, box_num) = b;
                let candidates = self.rows[*row] & self.cols[*col] & self.boxes[*box_num];
                candidates.count_ones()
            };

            a_count.cmp(&b_count)
        });

        let (row, col, box_num) = self.todo[index];

        let mut candidates = self.rows[row] & self.cols[col] & self.boxes[box_num];
        while candidates != 0 {
            let candidate = get_low_bit(candidates);
            self.rows[row] ^= candidate;
            self.cols[col] ^= candidate;
            self.boxes[box_num] ^= candidate;

            if index == self.todo.len() - 1 || self.solve(index + 1) {
                self.board[(row, col)] = pos_to_value(candidate);
                return true;
            }

            self.rows[row] |= candidate;
            self.cols[col] |= candidate;
            self.boxes[box_num] |= candidate;
            candidates = clear_low_bit(candidates);
        }
        false
    }

    #[cfg(feature = "unchecked_indexing")]
    fn solve(&mut self, index: usize) -> bool {
        unsafe {
            // Sort so the cell with the least number of candidates is first. The idea
            // being we can solve the easy ones first, then it helps to eliminate options
            // for cells with more possibilities
            #[cfg(feature = "priority_queue")]
            self.todo.get_unchecked_mut(index..).sort_by(|a, b| {
                let a_count = {
                    let (row, col, box_num) = *a;
                    let candidates = self.rows.get_unchecked(row)
                        & self.cols.get_unchecked(col)
                        & self.boxes.get_unchecked(box_num);
                    candidates.count_ones()
                };
                let b_count = {
                    let (row, col, box_num) = *b;
                    let candidates = self.rows.get_unchecked(row)
                        & self.cols.get_unchecked(col)
                        & self.boxes.get_unchecked(box_num);
                    candidates.count_ones()
                };

                a_count.cmp(&b_count)
            });

            let (row, col, box_num) = *self.todo.get_unchecked(index);

            let mut candidates = self.rows.get_unchecked(row)
                & self.cols.get_unchecked(col)
                & self.boxes.get_unchecked(box_num);
            while candidates != 0 {
                let candidate = get_low_bit(candidates);
                *self.rows.get_unchecked_mut(row) ^= candidate;
                *self.cols.get_unchecked_mut(col) ^= candidate;
                *self.boxes.get_unchecked_mut(box_num) ^= candidate;

                if index == self.todo.len() - 1 || self.solve(index + 1) {
                    self.board[(row, col)] = pos_to_value(candidate);
                    return true;
                }

                *self.rows.get_unchecked_mut(row) |= candidate;
                *self.cols.get_unchecked_mut(col) |= candidate;
                *self.boxes.get_unchecked_mut(box_num) |= candidate;
                candidates = clear_low_bit(candidates);
            }
            false
        }
    }

    fn is_solved(&self) -> bool {
        self.todo.is_empty()
    }
}

impl Default for Solver {
    fn default() -> Self {
        Self {
            board: Board([0; 81]),
            rows: [ALL_BITS_SET; 9],
            cols: [ALL_BITS_SET; 9],
            boxes: [ALL_BITS_SET; 9],
            todo: Vec::new(),
        }
    }
}

impl TryFrom<Board> for Solver {
    type Error = &'static str;

    fn try_from(board: Board) -> Result<Self, Self::Error> {
        let mut solver = Solver {
            board,
            ..Default::default()
        };

        for row in 0..9 {
            for col in 0..9 {
                let box_number = (row / 3) * 3 + (col / 3);
                let value = board[(row, col)];
                if value == 0 {
                    solver.todo.push((row, col, box_number));
                } else {
                    let mask: u16 = 1 << (value - 1);

                    // Because we are XORing the bits, if any of them are set to 0 then
                    // we have already seen it, and should bail because the board is invalid
                    if solver.rows[row] & solver.cols[col] & solver.boxes[box_number] & mask == 0 {
                        return Err("Invalid board");
                    }

                    solver.rows[row] ^= mask;
                    solver.cols[col] ^= mask;
                    solver.boxes[box_number] ^= mask;
                }
            }
        }

        Ok(solver)
    }
}

/// Get the lowest set bit of a [`u16`]
fn get_low_bit(value: u16) -> u16 {
    value & (0 - value)
}

/// Invert the lowest bit of a [`u16`]
fn clear_low_bit(value: u16) -> u16 {
    value & (value - 1)
}

/// Convert a [`u16`] with a single bit set into a [`u8`] value
/// representing the index of the bit set.
fn pos_to_value(pos: u16) -> u8 {
    match pos {
        1 => 1,
        2 => 2,
        4 => 3,
        8 => 4,
        16 => 5,
        32 => 6,
        64 => 7,
        128 => 8,
        256 => 9,
        _ => panic!("Invalid position: {pos}"),
    }
}

impl crate::SudokuSolver for Board {
    fn solve(&self) -> Option<Solution> {
        let mut solver = Solver::try_from(*self).ok()?;
        if solver.is_solved() {
            Some(*self)
        } else if solver.solve(0) {
            Some(solver.board)
        } else {
            None
        }
    }
}
