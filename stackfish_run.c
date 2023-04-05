#include <stdio.h>
#include "stackfish.c"

int main() {
    printf("Running stackfish\n");
    struct tetris_game game_thing = {0};
    init_game(&game_thing);

    display_game(&game_thing);
    
    int * possible_placements = get_possible_placements(game_thing.piece.type, game_thing.board);

    for (int i = 0; i < 3*36; i+=3) {
        printf("%d, %d, %d\n", possible_placements[i], possible_placements[i+1], possible_placements[i+2]);
    }
}