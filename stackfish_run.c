#include <stdio.h>
#include "stackfish.c"

int main() {
    printf("Running stackfish\n");
    srand(time(NULL));
    struct tetris_game game_thing = {0};
    init_game(&game_thing);
    // game_thing.piece.type = O_PIECE;
    int test_board[BOARD_H*BOARD_W] = {5, 5, 0, 2, 2, 2, 2, 0, 0, 6, 5, 5, 1, 1, 1, 0, 0, 0, 6, 6, 0, 0, 0, 1, 0, 0, 0, 0, 6, 4, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4};
    for (int i = 0; i<40;i++) {
        game_thing.board[i] = test_board[i];
    }
    game_thing.piece.type = L_PIECE;

    display_game(&game_thing);
    
    int * possible_placements = get_possible_placements(game_thing.piece.type, game_thing.board);

    for (int i = 0; possible_placements[i] != -1 && i < 3*36; i+=3) {
        printf("coords: %d, %d, %d\n", possible_placements[i], possible_placements[i+1], possible_placements[i+2]);
        game_thing.piece.x = possible_placements[i];
        game_thing.piece.y = possible_placements[i+1];
        game_thing.piece.r = possible_placements[i+2];
        display_game(&game_thing);
    }
}