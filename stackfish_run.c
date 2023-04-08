#include <stdio.h>
#include "stackfish.c"

int main() {
    printf("Running stackfish\n");
    srand(time(NULL));
    struct tetris_game game_thing = {0};
    struct tetris_game test_game = {0};
    
    init_game(&game_thing);
    test_game = game_thing;
    int test_board[BOARD_H*BOARD_W] = {2, 5, 5, 0, 0, 0, 0, 7, 7, 6, 2, 5, 5, 0, 0, 0, 7, 7, 6, 6, 2, 5, 5, 0, 0, 0, 0, 0, 6, 4, 2, 5, 5, 0, 0, 0, 0, 4, 4, 4};
    game_thing.piece.type = L_PIECE;
    game_thing.queue.pieces[game_thing.queue.index_shift] = T_PIECE;
    for (int i = 0; i < 40; i++) {
        game_thing.board[i] = test_board[i];
    }
    
    int * possible_placements = get_possible_placements(game_thing.piece.type, game_thing.board);

    int max_score = 0;
    for (int i = 0; possible_placements[i] != -1 && i < 3*36; i+=3) {
        test_game = game_thing;
        // printf("coords: %d, %d, %d\n", possible_placements[i], possible_placements[i+1], possible_placements[i+2]);
        test_game.piece.x = possible_placements[i];
        test_game.piece.y = possible_placements[i+1];
        test_game.piece.r = possible_placements[i+2];
        try_place_piece(&test_game);
        int evaluation = evaluate_game(test_game, 3);
        // printf("eval: %d\n", evaluation);
        if (evaluation > max_score) {
            max_score = evaluation;
            game_thing.piece.x = possible_placements[i];
            game_thing.piece.y = possible_placements[i+1];
            game_thing.piece.r = possible_placements[i+2];
        }
        swap_hold(&test_game);

        int hold_evaluation = evaluate_game(test_game, 2);

        if (hold_evaluation > max_score) {
            max_score = hold_evaluation;
            game_thing.piece.x = possible_placements[i];
            game_thing.piece.y = possible_placements[i+1];
            game_thing.piece.r = possible_placements[i+2];
        }

        // display_game(&test_game);
    }
    
    display_game(&game_thing);
    
    printf("eval: %d\n", max_score);

}