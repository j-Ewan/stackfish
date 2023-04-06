#include <stdio.h>
#include "stackfish.c"

int main() {
    printf("Running stackfish\n");
    srand(time(NULL));
    struct tetris_game game_thing = {0};
    init_game(&game_thing);
    // game_thing.piece.type = O_PIECE;
    int legs[BOARD_H*BOARD_W] = {L_PIECE, L_PIECE, L_PIECE, 0, 0, 0, 0, 0, 0, 0,
                        L_PIECE, Z_PIECE, Z_PIECE, 0, 0, 0, 0, 0, 0, 0,
                        O_PIECE, O_PIECE, Z_PIECE, Z_PIECE, 0, 0, 0, 0, 0, 0,
                        O_PIECE, O_PIECE, I_PIECE, I_PIECE, I_PIECE, I_PIECE, 0, 0, 0, 0};
    for (int i = 0; i<40;i++) {
        game_thing.board[i] = legs[39-i];
    }

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