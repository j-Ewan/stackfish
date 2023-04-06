#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "tetris_header.h"


int LAST_CALCED_MINOS[12]; // stores latest calculation in memory as an optimization

// gets an array of 4 2-tuples of the individual minos of the given piece
int * get_minos(struct active_piece piece) {
    if (piece.x == LAST_CALCED_MINOS[0]
     && piece.y == LAST_CALCED_MINOS[1]
     && piece.r == LAST_CALCED_MINOS[2]
     && piece.type == LAST_CALCED_MINOS[3]) {
        static int res[8];
        for (int i = 0; i < 8; i++) {
            res[i] = LAST_CALCED_MINOS[i+4];
        }
        // printf("optimization successful!   ");
        return res;
    }

    static int result[8];

    for (int i = 0; i < 8; i++) {
        result[i] = PIECE_SHAPES[piece.type][i];
        result[i] = PIECE_SHAPES[piece.type][i];
    }

    // apply rotation first
    for (int i = 0; i < 8; i+=2) {
        int temp = result[i];
        switch (piece.r) {
            case 1:
                result[i] = result[i+1];
                result[i+1] = -temp;
                break;
            case 2:
                result[i] *= -1;
                result[i+1] *= -1;
                break;
            case 3:
                result[i] = -result[i+1];
                result[i+1] = temp;
                break;
        }
    }

    // translation
    for (int i = 0; i < 8; i+=2) {
        result[i] += piece.x;
        result[i+1] += piece.y;
    }

    LAST_CALCED_MINOS[0] = piece.x;
    LAST_CALCED_MINOS[1] = piece.y;
    LAST_CALCED_MINOS[2] = piece.r;
    LAST_CALCED_MINOS[3] = piece.type;
    for (int i = 0; i < 8; i++) {
        LAST_CALCED_MINOS[i+4] = result[i];
    }

    return result;
} 

void add_color(int piece) {
    switch (piece) {
        case 0:
            printf("\033[0m");
            break;
        case T_PIECE:
            printf("\033[0;35m");
            break;
        case I_PIECE:
            printf("\033[0;36m");
            break;
        case L_PIECE:
            printf("\033[0;33m");
            break;
        case J_PIECE:
            printf("\033[0;34m");
            break;
        case O_PIECE:
            printf("\033[0;33m");
            break;
        case S_PIECE:
            printf("\033[0;32m");
            break;
        case Z_PIECE:
            printf("\033[0;31m");
            break;
    }
}

void display_game(struct tetris_game * game) {
    // hold piece
    add_color(game->hold_piece);
    printf("%c   ", PIECE_IDS[game->hold_piece]);
    add_color(0);

    // queue
    for (int i = 0; i < 5; i++) {
        int piece_type = game->queue.pieces[(i + game->queue.index_shift)%Q_LENGTH];
        add_color(piece_type);
        printf("%c ", PIECE_IDS[piece_type]);
        add_color(0);
    }

    int * piece_minos;
    piece_minos = get_minos(game->piece);

    printf("\n\n|--------------------|\n|");


    // check toptwo rows (only active piece) 
    for (int y = BOARD_H + 2; y >= BOARD_H; y--) {
        for (int x = 0; x < BOARD_W; x++)
        {
            int current_piece = 0;

            for (int m = 0; m < 8; m+=2) {
                if (piece_minos[m] == x && piece_minos[m+1] == y) {
                    current_piece = game->piece.type;
                }
            }
            add_color(current_piece);
            printf("%c ", PIECE_IDS[current_piece]);
            add_color(0);
        }
        printf("|\n|");
    }

    // board and active piece
    for (int y = BOARD_H - 1; y >= 0; y--) {
        for (int x = 0; x < BOARD_W; x++)
        {

            int current_piece = game->board[x + BOARD_W * y];

            for (int m = 0; m < 8; m+=2) {
                if (piece_minos[m] == x && piece_minos[m+1] == y) {
                    current_piece = game->piece.type;
                }
            }
            add_color(current_piece);
            printf("%c ", PIECE_IDS[current_piece]);
            add_color(0);
        }
        printf("|\n|");
    }
    printf("--------------------|\n");
}

int check_collision(struct active_piece piece, int board[BOARD_W*BOARD_H]) {    
    int * minos;
    minos = get_minos(piece);

    for (int i = 0; i < 8; i+=2) {
        if (minos[i] < 0 || minos[i] >= BOARD_W || minos[i + 1] < 0) return 1;
        if (minos[i + 1] >= BOARD_H) continue;
        if (board[ minos[i] + BOARD_W * (minos[i + 1]) ] != 0) return 1;
    }

    return 0; 
}

int in_bounds(struct active_piece piece, int board[BOARD_W*BOARD_H]) {    
    int * minos;
    minos = get_minos(piece);
    for (int i = 0; i < 8; i+=2) {
        if (minos[i] < 0 || minos[i] >= BOARD_W || minos[i + 1] < 0 || minos[i+1] >= BOARD_H) return 0;
    }

    return 1; 
}

int rotate_srs(struct active_piece * piece, int rotation, int board[BOARD_W*BOARD_H]) {
    // see: https://tetris.fandom.com/wiki/SRS#How_Guideline_SRS_Really_Works

    struct active_piece test_piece = {
        piece->x,
        piece->y,
        (piece->r + rotation)%4,
        piece->type
    };

    if (rotation % 4 == 2) {
        // 180
        for (int i = 0; i < 4; i+=2) {
            int shift_x;
            int shift_y;
            switch (piece->type) {
                case O_PIECE:
                    shift_x = KICK_TABLE_180_O[piece->r][i] - KICK_TABLE_180_O[test_piece.r][i];
                    shift_y = KICK_TABLE_180_O[piece->r][i+1] - KICK_TABLE_180_O[test_piece.r][i+1];
                    break;
                case I_PIECE:
                    shift_x = KICK_TABLE_180_I[piece->r][i] - KICK_TABLE_180_I[test_piece.r][i];
                    shift_y = KICK_TABLE_180_I[piece->r][i+1] - KICK_TABLE_180_I[test_piece.r][i+1];
                    break;
                default:
                    shift_x = KICK_TABLE_180[piece->r][i] - KICK_TABLE_180[test_piece.r][i];
                    shift_y = KICK_TABLE_180[piece->r][i+1] - KICK_TABLE_180[test_piece.r][i+1];
                    break;
            }

            test_piece.x = piece->x + shift_x;
            test_piece.y = piece->y + shift_y;
            if (check_collision(test_piece, board) == 0) {
                piece->x = test_piece.x;
                piece->y = test_piece.y;
                piece->r = test_piece.r;
                // printf("kick num %d", i/2);

                return 0;
            }
        }
    } else {
        // not 180
        for (int i = 0; i < 10; i+=2) {
            int shift_x;
            int shift_y;
            switch (piece->type) {
            case O_PIECE:
                shift_x = KICK_TABLE_O[piece->r][i] - KICK_TABLE_O[test_piece.r][i];
                shift_y = KICK_TABLE_O[piece->r][i+1] - KICK_TABLE_O[test_piece.r][i+1];
                break;
            case I_PIECE:
                shift_x = KICK_TABLE_I[piece->r][i] - KICK_TABLE_I[test_piece.r][i];
                shift_y = KICK_TABLE_I[piece->r][i+1] - KICK_TABLE_I[test_piece.r][i+1];
                break;
            default:
                shift_x = KICK_TABLE[piece->r][i] - KICK_TABLE[test_piece.r][i];
                shift_y = KICK_TABLE[piece->r][i+1] - KICK_TABLE[test_piece.r][i+1];
                break;
            }

            test_piece.x = piece->x + shift_x;
            test_piece.y = piece->y + shift_y;
            if (check_collision(test_piece, board) == 0) {
                piece->x = test_piece.x;
                piece->y = test_piece.y;
                piece->r = test_piece.r;
                // printf("kick num %d", i/2);
                return 0;
            }
        }
    }
    return 1;
}

void try_append_bag(struct piece_queue * queue) {
    // get index to append to and stop if not enough space
    int index;
    for (index = 0; queue->pieces[(index + queue->index_shift) % Q_LENGTH] != 0; index++) {
        if (Q_LENGTH - index <= 7) {
            // printf("queue too full too append\n");
            return;
        }
    }

    // printf("appending");

    int bag[7] = BAG;

    // randomize bag
    for (int i = 0; i < 6; i++) {
        int j = rand() % (7-i) + i;
        int temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }

    for (int i = 0; i < 7; i++) {
        queue->pieces[(index + i + queue->index_shift) % Q_LENGTH] = bag[i];
    }
}

void extract_piece_from_queue(struct tetris_game * game) {
    game->piece.type = game->queue.pieces[game->queue.index_shift];
    game->queue.pieces[game->queue.index_shift] = 0;
    game->queue.index_shift += 1;
    game->queue.index_shift %= Q_LENGTH;

    try_append_bag(&game->queue);
}

void reset_piece_pos(struct active_piece * piece) {
    piece->x = 4;
    piece->y = 4;
    piece->r = 0;
}

int clear_line(int *board, int line) {
    for (int l = line; l < BOARD_H - 1; l++) {
        // printf("Shifting line #%d to line # %d  ", l+1, l);
        for (int i = 0; i < BOARD_W; i++) {
            board[i + BOARD_W * l] = board[i + BOARD_W * (l + 1)];
        }
    }
    for (int i = 0; i < BOARD_W; i++) {
        board[BOARD_W*3 + i] = 0;
    }
    return 0;
}

int clear_filled_lines(int *board) {
    for (int line = BOARD_H - 1; line >= 0; line--) {
        int filled_minos = 0;
        for (int i = 0; i < BOARD_W; i++) {
            filled_minos += board[BOARD_W*line + i] != 0;
        }
        if (filled_minos == BOARD_W) {
            // printf("Clearing line #%d  ", line);
            clear_line(board, line);
        }
    }
}

int can_place_piece(struct active_piece piece, int board[BOARD_H*BOARD_W]){
    struct active_piece down_piece = {
        piece.x,
        piece.y - 1,
        piece.r,
        piece.type
    };
    return check_collision(down_piece, board) && !check_collision(piece, board) && in_bounds(piece, board);
}

void try_place_piece(struct tetris_game * game) {
    if (!can_place_piece(game->piece, game->board)) {return;}
    int * minos;
    minos = get_minos(game->piece);

    for (int i = 1; i < 8; i+=2) {
        if (minos[i] >= 4) {
            return;
        }
    }

    for (int i = 0; i < 8; i+=2) {
        game->board[minos[i] + BOARD_W * minos[i+1]] = game->piece.type;
    }
    
    extract_piece_from_queue(game);
    reset_piece_pos(&game->piece);
    game->cant_hold = 0;
    clear_filled_lines(game->board);
}

void try_translate_piece(struct active_piece * piece, int * board, int movement[2]) {
    struct active_piece test_piece = {
        piece->x + movement[0],
        piece->y + movement[1],
        piece->r,
        piece->type
    };

    if (check_collision(test_piece, board) == 1) {
        return;
    }

    piece->x = test_piece.x;
    piece->y = test_piece.y;
    piece->r = test_piece.r;

    return;
}

void swap_hold(struct tetris_game * game) {
    if (game->cant_hold) return;

    game->cant_hold = 1;
    reset_piece_pos(&game->piece);

    if (game->hold_piece == 0) {
        game->hold_piece = game->piece.type;
        extract_piece_from_queue(game);
        return;
    }
    
    int temp = game->hold_piece;
    game->hold_piece = game->piece.type;
    game->piece.type = temp;
}

void init_game(struct tetris_game * game) {
    
    memset(game, 0, sizeof(*game)); // clear all data
    
    // initilaize queue
    try_append_bag(&game->queue);
    try_append_bag(&game->queue);
    try_append_bag(&game->queue);

    // for (int i = 0; i < 21; i++) {
    //     printf("%c", PIECE_IDS[game->queue.pieces[(i + game->queue.index_shift)%Q_LENGTH]]);
    // }

    // initialize piece
    reset_piece_pos(&game->piece);
    extract_piece_from_queue(game);
}