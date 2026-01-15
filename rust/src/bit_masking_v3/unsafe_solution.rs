use crate::bit_masking_v3::Solver;

impl Solver {
    #[allow(unused)]
    #[inline(always)]
    fn find_best_index(&self, start: usize) -> usize {
        let mut best = start;
        let mut i = start;
        let mut best_count = !0;
        while i < self.todo.len() {
            let (row, col, box_num) = unsafe { *self.todo.get_unchecked(i) };
            let count = unsafe {
                (self.rows.get_unchecked(row)
                    & self.cols.get_unchecked(col)
                    & self.boxes.get_unchecked(box_num))
                .count_ones()
            };
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

    pub(super) fn solve(&mut self, index: usize) -> bool {
        unsafe {
            // Sort so the cell with the least number of candidates is first. The idea
            // being we can solve the easy ones first, then it helps to eliminate options
            // for cells with more possibilities
            #[cfg(feature = "constraint_solve_full_sort")]
            self.todo.get_unchecked_mut(index..).sort_by(|&a, &b| {
                let a_count = {
                    let (row, col, box_num) = a;
                    (self.rows.get_unchecked(row)
                        & self.cols.get_unchecked(col)
                        & self.boxes.get_unchecked(box_num))
                    .count_ones()
                };
                let b_count = {
                    let (row, col, box_num) = b;
                    (self.rows.get_unchecked(row)
                        & self.cols.get_unchecked(col)
                        & self.boxes.get_unchecked(box_num))
                    .count_ones()
                };

                a_count.cmp(&b_count)
            });
            #[cfg(feature = "constraint_solve_minimal_sort")]
            {
                let best = self.find_best_index(index);
                self.todo.swap(index, best);
            }

            let (row, col, box_num) = *self.todo.get_unchecked(index);

            let mut candidates = self.rows.get_unchecked(row)
                & self.cols.get_unchecked(col)
                & self.boxes.get_unchecked(box_num);
            while candidates != 0 {
                let candidate = crate::bit_masking_v3::get_low_bit(candidates);
                *self.rows.get_unchecked_mut(row) ^= candidate;
                *self.cols.get_unchecked_mut(col) ^= candidate;
                *self.boxes.get_unchecked_mut(box_num) ^= candidate;

                if index == self.todo.len() - 1 || self.solve(index + 1) {
                    self.board[(row, col)] = crate::bit_masking_v3::pos_to_value(candidate);
                    return true;
                }

                *self.rows.get_unchecked_mut(row) |= candidate;
                *self.cols.get_unchecked_mut(col) |= candidate;
                *self.boxes.get_unchecked_mut(box_num) |= candidate;
                candidates = crate::bit_masking_v3::clear_low_bit(candidates);
            }
            false
        }
    }

    pub(super) fn is_solved(&self) -> bool {
        self.todo.is_empty()
    }
}
