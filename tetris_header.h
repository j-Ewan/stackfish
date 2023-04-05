#ifndef HEADER_FILE
#define HEADER_FILE

#define BOARD_H 4
#define BOARD_W 10
#define Q_LENGTH 21
#define PIECE_IDS " TILJOSZ"
#define T_PIECE 1
#define I_PIECE 2
#define L_PIECE 3
#define J_PIECE 4
#define O_PIECE 5
#define S_PIECE 6
#define Z_PIECE 7
#define BAG {T_PIECE, I_PIECE, L_PIECE, J_PIECE, O_PIECE, S_PIECE, Z_PIECE}
// const int BAG[7] = {T_PIECE, I_PIECE, L_PIECE, J_PIECE, O_PIECE, S_PIECE, Z_PIECE};

// 2d arrays stored as 1d arrays
//               x1,y1, x2,y2, x3,y3, x4,y4
#define T_SHAPE {0, 0,  1, 0, -1, 0,  0, 1}
#define I_SHAPE {0, 0,  1, 0, -1, 0,  2, 0}
#define L_SHAPE {0, 0,  1, 0, -1, 0,  1, 1}
#define J_SHAPE {0, 0,  1, 0, -1, 0, -1, 1}
#define O_SHAPE {0, 0,  1, 0,  0, 1,  1, 1}
#define S_SHAPE {0, 0,  0, 1, -1, 0,  1, 1}
#define Z_SHAPE {0, 0,  1, 0,  0, 1, -1, 1}


const int PIECE_SHAPES[8][8] = {{0}, T_SHAPE, I_SHAPE, L_SHAPE, J_SHAPE, O_SHAPE, S_SHAPE, Z_SHAPE};

const int KICK_TABLE[4][10] = {
    { 0 },
    { 0, 0,  1, 0,  1,-1,  0, 2,  1, 2 },
    { 0 },
    { 0, 0, -1, 0, -1,-1,  0, 2, -1, 2 },
};

const int KICK_TABLE_I[4][10] = {
    { 0, 0, -1, 0,  2, 0, -1, 0,  2, 0 },
    {-1, 0,  0, 0,  0, 0,  0, 1,  0,-2 },
    {-1, 1,  1, 1, -2, 1,  1, 0, -2, 0 },
    { 0, 1,  0, 1,  0, 1,  0,-1,  0, 2 },
};

const int KICK_TABLE_O[4][2] = {
    { 0, 0 },
    { 0,-1 },
    {-1,-1 },
    {-1, 0 },
};

const int KICK_TABLE_180[4][4] = {
    { 0, 0,  0, 1 },
    { 0, 0,  1, 0 },
    { 0 },
    { 0 },
};

const int KICK_TABLE_180_I[4][4] = {
    { 1,-1,  1, 0 },
    {-1,-1,  0,-1 },
    { 0 },
    { 0 },
};

const int KICK_TABLE_180_O[4][2] = {
    { 1, 1 },
    { 1,-1 },
    { 0 },
    { 0 },
};


struct active_piece {
    int x;
    int y;
    int r; // num of clockwise rotations % 4
    int type;
};

// an array and an index shift; the shift avoids extensive memory setting;
// use queue[ (i + index_shift) % Q_LENGTH ] to get items
struct piece_queue {
    int pieces[Q_LENGTH];
    int index_shift; // optimization
};

struct tetris_game {
    int board[BOARD_H * BOARD_W];
    struct active_piece piece;
    int hold_piece;
    int cant_hold;
    struct piece_queue queue;
};

#endif