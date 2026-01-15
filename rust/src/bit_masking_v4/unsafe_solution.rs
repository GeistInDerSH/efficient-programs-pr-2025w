/// This module contains the same/similar implementation of the [`Solver`] functions
/// to try to solve a [`Board`] seen in `safe_solution`.
/// Generally, it's better to stick to the code that doesn't rely on `unsafe`, but it's
/// interesting to see how much of a performance difference it does (or does not) make.
use crate::bit_masking_v4::{
    clear_low_bit, get_low_bit, pos_to_value, BoxEntry, ColEntry, RowEntry, Solver,
};


impl Solver {
    #[inline(always)]
    fn find_best_index(&self, start: usize) -> usize {
        unsafe {
            let mut best = start;
            let mut best_count = !0;
            for (i, &(row, col, box_num)) in self.todo.get_unchecked(start..).iter().enumerate() {
                let mask = self.get_mask(row, col, box_num);
                let count = mask.count_ones();
                if count < best_count {
                    best = i;
                    best_count = count;
                    if best_count <= 1 {
                        break;
                    }
                }
            }
            start + best
        }
    }

    #[inline]
    fn get_mask(&self, row: RowEntry, col: ColEntry, box_num: BoxEntry) -> u16 {
        unsafe {
            self.rows.get_unchecked(row)
                & self.cols.get_unchecked(col)
                & self.boxes.get_unchecked(box_num)
        }
    }

    pub(super) fn solve(&mut self, index: usize) -> bool {
        unsafe {
            let best = self.find_best_index(index);
            // Interleave loading with the swap, to make better use of cache lines.
            let (row, col, box_num) = *self.todo.get_unchecked(best);
            self.todo.swap(index, best);

            let mut candidates = self.get_mask(row, col, box_num);
            while candidates != 0 {
                let candidate = get_low_bit(candidates);
                candidates = clear_low_bit(candidates);
                *self.rows.get_unchecked_mut(row) ^= candidate;
                *self.cols.get_unchecked_mut(col) ^= candidate;
                *self.boxes.get_unchecked_mut(box_num) ^= candidate;

                if index + 1 == self.todo.len() || self.solve(index + 1) {
                    self.board[row * 9 + col] = pos_to_value(candidate);
                    return true;
                }

                *self.rows.get_unchecked_mut(row) |= candidate;
                *self.cols.get_unchecked_mut(col) |= candidate;
                *self.boxes.get_unchecked_mut(box_num) |= candidate;
            }
            false
        }
    }
}
