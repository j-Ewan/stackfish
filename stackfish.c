#include "tetris_core.c"

// constants for board evaluation, mostly arbitrary for now
#define PERFECT_CLEAR_BONUS 1000000
const int BOARD_HEAT_MAP[BOARD_H*BOARD_W] =         {5, 5, 4, 4, 3, 3, 3, 3, 4, 5, 5, 5, 4, 3, 3, 2, 0, 1, 4, 5, 5, 4, 4, 2, 2, 1, 0, -1, 1, 4, 5, 4, 3, 3, 2, 1, -1, -1, 0, 4};
const int BOARD_HEAT_MAP_REVERSE[BOARD_H*BOARD_W] = {5, 4, 3, 3, 3, 3, 4, 4, 5, 5, 5, 4, 1, 0, 2, 3, 3, 4, 5, 5, 4, 1, -1, 0, 1, 2, 2, 4, 4, 5, 4, 0, -1, -1, 1, 2, 3, 3, 4, 5};

// gets a list of all possible piece placements given a piece and board. return array in the form of 36 3-tuples, (x, y, r)
int * get_possible_placements(int piece_type, int board[BOARD_H*BOARD_W]) {
    int * placements = (int*) malloc(3*36 * sizeof(int));
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
    int score = 1000; // arbitrary base value

    // check for holes
    int empty_mino_count = 0;
    for (int row = 0; row < BOARD_W-1; row++) {
        for (int y = 0; y < 4; y++) empty_mino_count += board[row + y*10] == 0;
        if (!((board[row + 0*BOARD_W] == 0 && board[row+1 + 0*BOARD_W] == 0)
           || (board[row + 1*BOARD_W] == 0 && board[row+1 + 1*BOARD_W] == 0)
           || (board[row + 2*BOARD_W] == 0 && board[row+1 + 2*BOARD_W] == 0)
           || (board[row + 3*BOARD_W] == 0 && board[row+1 + 3*BOARD_W] == 0))) {
            if (empty_mino_count % 4 != 0) return 0;
            empty_mino_count = 0;
        }
    }

    // heat map (weight better boards)
    int map_score_left = 0;
    int map_score_right = 0;
    for (int i = 0; i < BOARD_H*BOARD_W; i++) {
        map_score_left += (board[i] != 0) * BOARD_HEAT_MAP[i];
        map_score_right += (board[i] != 0) * BOARD_HEAT_MAP_REVERSE[i];
    }
    int use_left = map_score_left > map_score_right;
    score += use_left * map_score_left
          + !use_left * map_score_right;
    

    return score;
}

struct uint64_int_dynamic_map {
    int length;
    int used;
    unsigned long * uint64s;
    int * ints;
};

void init_uint64_int_dynamic_map(struct uint64_int_dynamic_map * map, int length) {
    map->length = length;
    map->used = 0;
    map->uint64s = (unsigned long*) malloc(length * sizeof(unsigned long));
    map->ints = (int*) malloc(length * sizeof(int));
}

void del_uint64_int_dynamic_map(struct uint64_int_dynamic_map * map) {
    free(map->uint64s);
    free(map->ints);
}

void expand_uint64_int_dynamic_map(struct uint64_int_dynamic_map * map, double growth_factor) {
    int old_size = map->length;
    map->length *= growth_factor;
    unsigned long * new_mem_block_uint64s = (unsigned long*) realloc(map->uint64s, sizeof(unsigned long) * map->length);
    int * new_mem_block_ints = (int*) realloc(map->ints, sizeof(int) * map->length);

    if (new_mem_block_uint64s != NULL) {
        map->uint64s = new_mem_block_uint64s;
        memset(map->uint64s + old_size, 0, sizeof(unsigned long) * (map->length - old_size));
    } else printf("ERROR: failed to realloc uint64_int_dynamic_map.uint64s");
    if (new_mem_block_ints != NULL) {
        map->ints = new_mem_block_ints;
        memset(map->ints + old_size, 0, sizeof(int) * (map->length - old_size));
    } else printf("ERROR: failed to realloc uint64_int_dynamic_map.ints");
}

int get_uint64_int_dynamic_map(struct uint64_int_dynamic_map * map, unsigned long uint64) {
    for (int i = 0; i < map->used; i++) {
        if (map->uint64s[i] == uint64) return map->ints[i];
    }
    return -1;
}

void add_uint64_int_dynamic_map(struct uint64_int_dynamic_map * map, unsigned long uint64, int val) {
    if (map->length == map->used) {
        expand_uint64_int_dynamic_map(map, 1.25);
    }
    map->uint64s[map->used] = uint64;
    map->ints[map->used] = val;
    map->used++;
}

// encodes critical info of a game into a uint64
// 3 buffer bits, 40 bits for the board, 3 for hold piece, 3 for active, 15 for each piece in queue
unsigned long encode_game(struct tetris_game game) {
    unsigned long uint64 = 0;
    for (int i = 0; i < 40; i++) {
        uint64 <<= 1;
        uint64 |= game.board[i] != 0;
    }
    uint64 <<= 3;
    uint64 |= game.hold_piece;
    uint64 <<= 3;
    uint64 |= game.piece.type;
    for (int i = 0; i < 5; i++) {
        uint64 <<= 3;
        uint64 |= game.queue.pieces[i + game.queue.index_shift];
    }

    return uint64;
}

int evaluate_game(struct tetris_game game, int depth, struct uint64_int_dynamic_map * already_evaluated) {
    // check for perfect clear
    int mino_count = 0;
    for (int i = 0; i < BOARD_H*BOARD_W; i++) {
        mino_count += game.board[i];
    }

    int rough_evaluation = roughly_evaluate_board(game.board);
    if (depth == 0) {
        return rough_evaluation + (mino_count == 0) * PERFECT_CLEAR_BONUS;
    }
    if (rough_evaluation == 0) return 0;

    unsigned long encoded_game = encode_game(game);
    int check_already_evaluated = get_uint64_int_dynamic_map(already_evaluated, encoded_game);
    if (check_already_evaluated != -1) {
        return check_already_evaluated;
    }

    struct tetris_game test_game;
    int * possible_placements = get_possible_placements(game.piece.type, game.board);

    int max_score = 0;
    for (int i = 0; possible_placements[i] != -1 && i < 3*36; i+=3) {
        test_game = game;
        test_game.piece.x = possible_placements[i];
        test_game.piece.y = possible_placements[i+1];
        test_game.piece.r = possible_placements[i+2];

        try_place_piece(&test_game);

        int evaluation = evaluate_game(test_game, depth - 1, already_evaluated);

        if (evaluation > max_score) {
            max_score = evaluation;
        }

        swap_hold(&test_game);

        int hold_evaluation = evaluate_game(test_game, depth - 1, already_evaluated);

        if (hold_evaluation > max_score) {
            max_score = hold_evaluation;
        }

    }
    int result = max_score + (mino_count == 0) * PERFECT_CLEAR_BONUS;

    add_uint64_int_dynamic_map(already_evaluated, encoded_game, result);
    return result;
}

struct active_piece get_optimal_move(struct tetris_game game, int depth) {
    struct uint64_int_dynamic_map * already_calculated = (struct uint64_int_dynamic_map *) malloc(sizeof(struct uint64_int_dynamic_map));
    init_uint64_int_dynamic_map(already_calculated, 50);

    struct tetris_game test_game = {0};

    int * possible_placements = get_possible_placements(game.piece.type, game.board);

    int max_score = 0;
    struct active_piece best_piece;

    for (int i = 0; possible_placements[i] != -1 && i < 3*36; i+=3) {
        test_game = game;

        test_game.piece.x = possible_placements[i];
        test_game.piece.y = possible_placements[i+1];
        test_game.piece.r = possible_placements[i+2];

        try_place_piece(&test_game);
        int evaluation = evaluate_game(test_game, depth, already_calculated);
        if (evaluation > max_score) {
            max_score = evaluation;
            best_piece.x = possible_placements[i];
            best_piece.y = possible_placements[i+1];
            best_piece.r = possible_placements[i+2];
            best_piece.type = game.piece.type;
        }
        swap_hold(&test_game);

        int hold_evaluation = evaluate_game(test_game, depth, already_calculated);
        if (hold_evaluation > max_score) {
            max_score = hold_evaluation;
            best_piece.x = possible_placements[i];
            best_piece.y = possible_placements[i+1];
            best_piece.r = possible_placements[i+2];
            best_piece.type = game.piece.type;
        }
    }

    swap_hold(&game);
    possible_placements = get_possible_placements(game.piece.type, game.board);

    for (int i = 0; possible_placements[i] != -1 && i < 3*36; i+=3) {
        test_game = game;

        test_game.piece.x = possible_placements[i];
        test_game.piece.y = possible_placements[i+1];
        test_game.piece.r = possible_placements[i+2];

        try_place_piece(&test_game);
        int evaluation = evaluate_game(test_game, depth, already_calculated);
        if (evaluation > max_score) {
            max_score = evaluation;
            best_piece.x = possible_placements[i];
            best_piece.y = possible_placements[i+1];
            best_piece.r = possible_placements[i+2];
            best_piece.type = game.piece.type;
        }
        swap_hold(&test_game);

        int hold_evaluation = evaluate_game(test_game, depth, already_calculated);
        if (hold_evaluation > max_score) {
            max_score = hold_evaluation;
            best_piece.x = possible_placements[i];
            best_piece.y = possible_placements[i+1];
            best_piece.r = possible_placements[i+2];
            best_piece.type = game.piece.type;
        }
    }
    del_uint64_int_dynamic_map(already_calculated);
    free(already_calculated);

    return best_piece;
}