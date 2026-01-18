#include "sudoku_dlx.hpp"
#include "../common/board.hpp"

#include <fstream>
#include <cstdint>
#include <iostream>

// --------------------------------------------------
// Configuração DLX

static constexpr int N = 9;
static constexpr int COLS = 324;
static constexpr int MAX_NODES = 4000;

// --------------------------------------------------

struct Node {
    int L, R, U, D;
    int C;
    int row_id;
};

struct Column {
    int head;
    int size;
};

// --------------------------------------------------

static Node nodes[MAX_NODES];
static Column columns[COLS];
static int node_count;
static int root;

static int solution_rows[81];
static int solution_size;

// --------------------------------------------------

static inline int box_index(int r, int c) {
    return (r / 3) * 3 + (c / 3);
}

// --------------------------------------------------
// Node helpers

static int new_node(int col, int row_id) {
    int id = node_count++;
    nodes[id].C = col;
    nodes[id].row_id = row_id;
    return id;
}

// --------------------------------------------------
// DLX operations

static void cover(int c) {
    int col_head = columns[c].head;

    nodes[nodes[col_head].R].L = nodes[col_head].L;
    nodes[nodes[col_head].L].R = nodes[col_head].R;

    for (int r = nodes[col_head].D; r != col_head; r = nodes[r].D) {
        for (int n = nodes[r].R; n != r; n = nodes[n].R) {
            nodes[nodes[n].D].U = nodes[n].U;
            nodes[nodes[n].U].D = nodes[n].D;
            columns[nodes[n].C].size--;
        }
    }
}

static void uncover(int c) {
    int col_head = columns[c].head;

    for (int r = nodes[col_head].U; r != col_head; r = nodes[r].U) {
        for (int n = nodes[r].L; n != r; n = nodes[n].L) {
            columns[nodes[n].C].size++;
            nodes[nodes[n].D].U = n;
            nodes[nodes[n].U].D = n;
        }
    }

    nodes[nodes[col_head].R].L = col_head;
    nodes[nodes[col_head].L].R = col_head;
}

static int choose_column() {
    int best = -1;
    int min_size = 1e9;

    for (int c = nodes[root].R; c != root; c = nodes[c].R) {
        int col = nodes[c].C;
        if (columns[col].size < min_size) {
            min_size = columns[col].size;
            best = col;
        }
    }
    return best;
}

// --------------------------------------------------

static bool search() {
    if (nodes[root].R == root)
        return true;

    int c = choose_column();
    if (c < 0 || columns[c].size == 0)
        return false;

    cover(c);

    int col_head = columns[c].head;
    for (int r = nodes[col_head].D; r != col_head; r = nodes[r].D) {
        solution_rows[solution_size++] = nodes[r].row_id;

        for (int n = nodes[r].R; n != r; n = nodes[n].R)
            cover(nodes[n].C);

        if (search())
            return true;

        for (int n = nodes[r].L; n != r; n = nodes[n].L)
            uncover(nodes[n].C);

        solution_size--;
    }

    uncover(c);
    return false;
}

// --------------------------------------------------
// DLX construction

static void init_dlx() {
    node_count = 0;
    solution_size = 0;

    // root
    root = new_node(-1, -1);
    nodes[root].L = nodes[root].R = root;
    nodes[root].U = nodes[root].D = root;

    // columns
    for (int i = 0; i < COLS; i++) {
        int h = new_node(i, -1);
        columns[i].head = h;
        columns[i].size = 0;

        nodes[h].U = nodes[h].D = h;

        // link into root row
        nodes[h].R = nodes[root].R;
        nodes[h].L = root;
        nodes[nodes[root].R].L = h;
        nodes[root].R = h;
    }
}

static void add_row(int row_id, int c1, int c2, int c3, int c4) {
    int cols[4] = {c1, c2, c3, c4};
    int first = -1;

    for (int i = 0; i < 4; i++) {
        int col = cols[i];
        int n = new_node(col, row_id);

        // vertical
        int h = columns[col].head;
        nodes[n].D = h;
        nodes[n].U = nodes[h].U;
        nodes[nodes[h].U].D = n;
        nodes[h].U = n;
        columns[col].size++;

        // horizontal
        if (first == -1) {
            first = n;
            nodes[n].L = nodes[n].R = n;
        } else {
            nodes[n].R = first;
            nodes[n].L = nodes[first].L;
            nodes[nodes[first].L].R = n;
            nodes[first].L = n;
        }
    }
}

// --------------------------------------------------
// Column mapping

static inline int col_cell(int r, int c) { return r * 9 + c; }
static inline int col_row(int r, int v)  { return 81 + r * 9 + v; }
static inline int col_col(int c, int v)  { return 162 + c * 9 + v; }
static inline int col_box(int b, int v)  { return 243 + b * 9 + v; }

// --------------------------------------------------
// Solver

int solve(const Board& input, Board& solution) {
    init_dlx();
    solution = input;

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int cell = input.cells[r * 9 + c];
            int b = box_index(r, c);

            if (cell != 0) {
                int v = cell - 1;
                int row_id = r * 81 + c * 9 + v;

                add_row(row_id,
                    col_cell(r, c),
                    col_row(r, v),
                    col_col(c, v),
                    col_box(b, v)
                );
            } else {
                for (int v = 0; v < 9; v++) {
                    int row_id = r * 81 + c * 9 + v;

                    add_row(row_id,
                        col_cell(r, c),
                        col_row(r, v),
                        col_col(c, v),
                        col_box(b, v)
                    );
                }
            }
        }
    }

    if (!search())
        return 0;

    // decode
    for (int i = 0; i < solution_size; i++) {
        int id = solution_rows[i];
        int r = id / 81;
        int rem = id % 81;
        int c = rem / 9;
        int v = rem % 9;

        solution.cells[r * 9 + c] = v + 1;
    }

    return 1;
}

// --------------------------------------------------
// File IO

int read_file(Board& board, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        return 1;

    int idx = 0;
    char c;
    while (idx < 81 && file.get(c)) {
        if (c >= '0' && c <= '9')
            board.cells[idx++] = static_cast<uint8_t>(c - '0');
    }
    return idx == 81 ? 0 : 1;
}

void print_board(const Board& board) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++)
            std::cout << int(board.cells[r * 9 + c]);
        std::cout << "\n";
    }
}
