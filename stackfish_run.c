#include <stdio.h>
#include "stackfish.c"

int main() {
    printf("Running stackfish\n");
    srand(time(NULL));
    struct tetris_game game_thing = {0};
    
    init_game(&game_thing);
    // extract_piece_from_queue(&game_thing);
    // extract_piece_from_queue(&game_thing);
    // extract_piece_from_queue(&game_thing);
    // extract_piece_from_queue(&game_thing); // 7th pc
    int test_board[BOARD_H*BOARD_W] = {0, 0, 0, 0, 0, 0, 2, 2, 2, 2,  0, 0, 0, 0, 0, 0, 0, 7, 0, 0,  0, 0, 0, 0, 0, 0, 0, 7, 7, 0,  0, 0, 0, 0, 0, 0, 0, 0, 7, 0};
    game_thing.piece.type = J_PIECE;
    // game_thing.queue.pieces[game_thing.queue.index_shift] = T_PIECE;
    for (int i = 0; i < 40; i++) {
        game_thing.board[i] = test_board[i];
    }

    // int * p = get_possible_placements(game_thing.piece.type, game_thing.board);

    // for (int i = 0; p[i] != -1 && i < 3*36; i+= 3) {
    //     game_thing.piece.x = p[i];
    //     game_thing.piece.y = p[i+1];
    //     game_thing.piece.r = p[i+2];
    //     display_game(&game_thing);
    // }


    for (int ai_turn = 0; ai_turn < 20; ai_turn++) {
        display_game(&game_thing);

        struct active_piece ai_choice = get_optimal_move(game_thing, 4);

        if (ai_choice.type != game_thing.piece.type) swap_hold(&game_thing);

        game_thing.piece = ai_choice;

        // display_game(&game_thing);

        try_place_piece(&game_thing);
    }
    display_game(&game_thing);
}