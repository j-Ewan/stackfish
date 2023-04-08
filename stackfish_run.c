#include <stdio.h>
#include "stackfish.c"

int main() {
    printf("Running stackfish\n");
    srand(time(NULL));
    struct tetris_game game_thing = {0};
    
    init_game(&game_thing);
    int test_board[BOARD_H*BOARD_W] = {4, 4, 4, 0, 0, 0, 0, 7, 7, 1, 4, 5, 5, 0, 0, 0, 7, 7, 1, 1, 3, 5, 5, 0, 0, 0, 0, 6, 6, 1, 3, 3, 3, 0, 0, 0, 0, 0, 6, 6};
    game_thing.piece.type = I_PIECE;
    // // game_thing.queue.pieces[game_thing.queue.index_shift] = T_PIECE;
    for (int i = 0; i < 40; i++) {
        game_thing.board[i] = test_board[i];
    }

    for (int ai_turn = 0; ai_turn < 4; ai_turn++) {
        display_game(&game_thing);

        struct active_piece ai_choice = get_optimal_move(game_thing, 3-ai_turn);

        if (ai_choice.type != game_thing.piece.type) swap_hold(&game_thing);

        game_thing.piece = ai_choice;

        // display_game(&game_thing);

        try_place_piece(&game_thing);
    }
    display_game(&game_thing);
}