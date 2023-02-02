#include "tetris_core.c"


int main() {
    srand(time(NULL));

    struct tetris_game game = {0};
   
    init_game(&game);

    char action;
    while (action != 'q') {
        display_game(&game);

        scanf(" %c", &action);

        int movement[2] = {0};
        switch (action) {
            case 's':
                movement[1] = -1;
                break;
            case 'w':
                movement[1] = 1;
                break;
            case 'a':
                movement[0] = -1;
                break;
            case 'd':
                movement[0] = 1;
                break;
            case 'c':
                rotate_srs(&game.piece, 1, game.board);
                break;
            case 'x':
                rotate_srs(&game.piece, 3, game.board);
                break;
            case 'z':
                rotate_srs(&game.piece, 2, game.board);
                break;
            case 'v':
                try_place_piece(&game);
                break;
            case 'r':
                init_game(&game);
                break;
            case 'e':
                swap_hold(&game);
                break;
            case 't': // for tests
                
                break;

        }
        try_move_piece(&game, movement);
    }
    return 0;
}