use crate::bit_masking_v4::{
    clear_low_bit, count_ones, get_low_bit, pos_to_value, BoxEntry, ColEntry, RowEntry, Solver,
};

impl Solver {
    #[inline(always)]
    fn find_best_index(&self, start: usize) -> usize {
        let mut best = start;
        let mut i = start;
        let mut best_count = !0;
        while i < self.todo.len() {
            let (row, col, box_num) = self.todo[i];
            let mask = self.get_mask(row, col, box_num);
            let count = count_ones(mask);
            if count < best_count {
                best = i;
                best_count = count;
                if best_count <= 1 {
                    break;
                }
            }
            i += 1;
        }
        best
    }

    #[inline]
    fn get_mask(&self, row: RowEntry, col: ColEntry, box_num: BoxEntry) -> u16 {
        self.rows[row] & self.cols[col] & self.boxes[box_num]
    }

    pub(super) fn solve(&mut self, index: usize) -> bool {
        let best = self.find_best_index(index);
        // Interleave loading with the swap, to make better use of cache lines.
        let (row, col, box_num) = self.todo[best];
        self.todo.swap(index, best);

        let mut candidates = self.get_mask(row, col, box_num);
        while candidates != 0 {
            let candidate = get_low_bit(candidates);
            candidates = clear_low_bit(candidates);
            self.rows[row] ^= candidate;
            self.cols[col] ^= candidate;
            self.boxes[box_num] ^= candidate;

            if index + 1 == self.todo.len() || self.solve(index + 1) {
                self.board[row * 9 + col] = pos_to_value(candidate);
                return true;
            }

            self.rows[row] |= candidate;
            self.cols[col] |= candidate;
            self.boxes[box_num] |= candidate;
        }
        false
    }
}
