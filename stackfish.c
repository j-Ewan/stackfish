#include "tetris_core.c"

// gets a list of all possible piece placements given a piece and board. return array in the form of 36 3-tuples, (x, y, r)
int * get_possible_placements(int piece_type, int board[BOARD_H*BOARD_W]) {
    int * placements = (int*) malloc(3*64 * sizeof(int));
    memset(placements, -1, 3*36 * sizeof(int));
    int placements_count = 0;

    // an array where the position and rotation of the piece is encoded in the place of the array, and the value at the position represents which loop to check it on (0 means unchecked)
    // index with x*20 + y*4 + r
    char * when_to_check = (char*) malloc(4 * 5 * 10 * sizeof(char));
    memset(when_to_check, 0, 4*5*10*sizeof(char));
    int check_next_count = 0;

    struct active_piece test_piece = {0};
    test_piece.type = piece_type;
    int movement[2];

    // init first set of positions
    for (int r = 0; r < 4; r++) {
        int y = 4; 
        for (int x = 0; x < 10; x++) {
            when_to_check[x*20 + 4*4 + r] = 1;
            check_next_count++;
        }
    }

    // main loop
    for (int loop_num = 1; check_next_count != 0; loop_num++) {
        check_next_count = 0;
        
        // apply all possible movements to each piece
        for (int x = 0; x < 10; x++) {
        for (int y = 0; y < 5; y++) {
        for (int r = 0; r < 4; r++) {
            // need to check?
            if (when_to_check[x*20 + y*4 + r] != loop_num) continue;
            test_piece.x = x;
            test_piece.y = y;
            test_piece.r = r;

            // after first loop, check if viable placement
            if (loop_num > 1 && can_place_piece(test_piece, board)) {
                placements[placements_count*3    ] = x;
                placements[placements_count*3 + 1] = y;
                placements[placements_count*3 + 2] = r;
                placements_count++;
            }

            // translations
            movement[0] = 0; // initialize movement vector
            movement[1] = 1; // starts upward, but immediately rotates, so only checks other 3 directions
            for (int i = 0; i < 3; i++) {
                int temp = movement[0]; // rotate movement vector every loop
                movement[0] = movement[1];
                movement[1] = -temp;

                test_piece.x = x;
                test_piece.y = y;
                test_piece.r = r;

                try_translate_piece(&test_piece, board, movement);

                if (test_piece.x == x && test_piece.y == y /*&& test_piece.r == positions_to_check[i+2]*/)
                    continue; // didn't move, so skip
                if (when_to_check[test_piece.x*20 + test_piece.y*4 + test_piece.r] == 0) { // not checked yet?
                    when_to_check[test_piece.x*20 + test_piece.y*4 + test_piece.r] = loop_num + 1; // check it next
                    check_next_count++;
                }
                
            }

            // rotations
            for (int rot = 1; rot <= 3; rot++) {
                test_piece.x = x;
                test_piece.y = y;
                test_piece.r = r;

                int failed_rotate = rotate_srs(&test_piece, rot, board);

                if (failed_rotate) continue;
                if (test_piece.y > 4) continue;

                if (when_to_check[test_piece.x*20 + test_piece.y*4 + test_piece.r] == 0) { // not checked yet?
                    when_to_check[test_piece.x*20 + test_piece.y*4 + test_piece.r] = loop_num + 1; // check it next
                    check_next_count++;
                }
            }
        }}}
    }

    free(when_to_check);

    // printf("Placements count: %d\n", placements_count);
    return placements;
}

int roughly_evaluate_board(int board[BOARD_H * BOARD_W]) {
    // check for holes
    int empty_mino_count = 0;
    for (int row = 0; row < BOARD_W-1; row++) {
        for (int y = 0; y < 4; y++) empty_mino_count += board[row + y*10] == 0;
        if (!((board[row + 0*10] == 0 && board[row+1 + 0*10] == 0)
           || (board[row + 1*10] == 0 && board[row+1 + 1*10] == 0)
           || (board[row + 2*10] == 0 && board[row+1 + 2*10] == 0)
           || (board[row + 3*10] == 0 && board[row+1 + 3*10] == 0))) {
            if (empty_mino_count % 4 != 0) return 0;
            empty_mino_count = 0;
        }
    }
    return 1;
}

int evaluate_game(struct tetris_game game, int depth) {
    if (depth == 0) {
        return roughly_evaluate_board(game.board);
    }
}