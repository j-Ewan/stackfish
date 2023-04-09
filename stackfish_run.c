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
    // int test_board[BOARD_H*BOARD_W] = {6, 6, 0, 0, 0, 0, 0, 4, 4, 4, 1, 6, 6, 0, 0, 0, 0, 4, 5, 5, 1, 1, 0, 0, 0, 0, 0, 0, 5, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    // game_thing.piece.type = I_PIECE;
    // // game_thing.queue.pieces[game_thing.queue.index_shift] = T_PIECE;
    // for (int i = 0; i < 40; i++) {
    //     game_thing.board[i] = test_board[i];
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