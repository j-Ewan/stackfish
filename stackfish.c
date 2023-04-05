#include "tetris_core.c"

// gets a list of all possible piece placements given a piece and board. return array in the form of 36 3-tuples, (x, y, r)
int * get_possible_placements(int piece_type, int board[BOARD_H*BOARD_W]) {
    int * placements = (int*) malloc(3*36 * sizeof(int));
    memset(placements, -1, 3*36 * sizeof(int));
    // int placements[3 * 36] = { 0 }; // 3-tuples of x, y, rot; 36 is likely the max amount of placements possible
    int placements_count = 0;

    int * checked_positions = (int*) malloc(3 * BOARD_H * BOARD_W * 4 * sizeof(int)); // 3-tuples of all positions that have already been checked; no need to check again
    memset(checked_positions, -1, 3 * BOARD_H * BOARD_W * 4 * sizeof(int));
    int count_pos_checked = 0;

    int * positions_to_check = (int*) malloc(3 * 120 * sizeof(int));
    memset(positions_to_check, -1, 3 * 120 * sizeof(int));

    int * check_next = (int*) malloc(3 * 120 * sizeof(int));
    memset(check_next, -1, 3 * 120 * sizeof(int));
    int check_next_index = 0;

    struct active_piece test_piece = {0};
    test_piece.type = piece_type;
    int movement[2];


    int already_checked(int x, int y, int r) {
        for (int i = 0; checked_positions[i] == -1 || i < sizeof(*checked_positions)/sizeof(checked_positions[0]); i += 3) {
            if (checked_positions[i] == x 
             && checked_positions[i+1] == y
             && checked_positions[i+2] == r) {
                return 1;
            }
        }
        return 0;
    }

    int already_going_to_check(int x, int y, int r) {
        for (int i = 0; i <= check_next_index || i < sizeof(*check_next)/sizeof(check_next[0]); i += 3) {
            if (check_next[i] == x 
             && check_next[i+1] == y
             && check_next[i+2] == r) {
                return 1;
            }
        }
        return 0;
    }

    int already_discovered_placement(int x, int y, int r) {
        for (int i = 0; placements[i] == -1 || i < sizeof(*placements)/sizeof(placements[0]); i += 3) {
            if (placements[i]   == x 
             && placements[i+1] == y
             && placements[i+2] == r) {
                return 1;
            }
        }
        return 0;
    }

    // init first set of positions
    for (int r = 0; r < 4; r++) {
        for (int x = 0; x < 10; x++) {
            positions_to_check[r*10*3 + x*3    ] = x;
            positions_to_check[r*10*3 + x*3 + 1] = 4;
            positions_to_check[r*10*3 + x*3 + 2] = r;
        }
    }

    // main loop
    while (positions_to_check[0] != -1) {
        // apply all possible movements to each piece
        for (int i = 0; positions_to_check[i] != -1; i+=3) {
            // mark as checked
            checked_positions[count_pos_checked*3]   = positions_to_check[i];
            checked_positions[count_pos_checked*3+1] = positions_to_check[i+1];
            checked_positions[count_pos_checked*3+2] = positions_to_check[i+2];
            count_pos_checked++;

            // translations
            movement[0] = 0; // initialize movement vector
            movement[1] = 1; // starts upward, but immediately rotates, so only checks other 3 directions
            for (int i = 0; i < 3; i++) {
                int temp = movement[0]; // rotate movement vector every loop
                movement[0] = movement[1];
                movement[1] = -temp;

                test_piece.x = positions_to_check[i];
                test_piece.y = positions_to_check[i+1];
                test_piece.r = positions_to_check[i+2];

                try_translate_piece(&test_piece, board, movement);

                if (test_piece.x == positions_to_check[i] && test_piece.y == positions_to_check[i+1] /*&& test_piece.r == positions_to_check[i+2]*/)
                    continue; // didn't move, so skip
                
                if (already_checked(test_piece.x, test_piece.y, test_piece.r)) continue;
                if (already_going_to_check(test_piece.x, test_piece.y, test_piece.r)) continue;
                check_next[check_next_index  ] = test_piece.x;
                check_next[check_next_index+1] = test_piece.y;
                check_next[check_next_index+2] = test_piece.r;
                check_next_index += 3;

                if (!piece_on_ground(test_piece, board)) continue;

                if (already_discovered_placement(test_piece.x, test_piece.y, test_piece.r)) continue;
                placements[placements_count*3  ] = test_piece.x;
                placements[placements_count*3+1] = test_piece.y;
                placements[placements_count*3+2] = test_piece.r;
                placements_count++;
                printf("discovered placement %d\n", placements_count);
                
            }

            // rotations
            for (int rot = 1; rot <= 3; rot++) {
                test_piece.x = positions_to_check[i];
                test_piece.y = positions_to_check[i+1];
                test_piece.r = positions_to_check[i+2];

                int failed_rotate = rotate_srs(&test_piece, rot, board);

                if (failed_rotate) continue;                
                if (already_checked(test_piece.x, test_piece.y, test_piece.r)) continue;
                if (already_going_to_check(test_piece.x, test_piece.y, test_piece.r)) continue;

                check_next[check_next_index  ] = test_piece.x;
                check_next[check_next_index+1] = test_piece.y;
                check_next[check_next_index+2] = test_piece.r;
                check_next_index += 3;

                if (!piece_on_ground(test_piece, board)) continue;

                if (already_discovered_placement(test_piece.x, test_piece.y, test_piece.r)) {printf("whoops, rediscovered\n");continue;};
                placements[placements_count*3  ] = test_piece.x;
                placements[placements_count*3+1] = test_piece.y;
                placements[placements_count*3+2] = test_piece.r;
                placements_count++;
                printf("discovered placement %d\n", placements_count);
            }
        }


        printf("checking next: %d\n", check_next_index/3);
        if (check_next_index == 0) { // no placements to check next
            break;
        }
        // swap pointers instead of large lists
        int * temp = positions_to_check;
        positions_to_check = check_next;
        check_next = temp;
        check_next_index = 0;

        for (int i = 0; i < 3*36; i+=3) {
            printf("%d, %d, %d\n", positions_to_check[i], positions_to_check[i+1], positions_to_check[i+2]);
        }
        // printf("first to check next: %d, %d, %d\n", positions_to_check[0], positions_to_check[1], positions_to_check[2]);
    }



    /** Plan:
     * init list of checked positions
     * spawn copies of main piece above board in all orientations in all columns (check each to make sure it is within bounds)
     * spawn copies for each possible movement of the piece, only proceed if it is new
     *    mark as checked
     *    if it collides, delete it
     * when no more new positions are being checked, "harddrop" all and remove dupes
     * return list
     */

    free(checked_positions);
    free(positions_to_check);
    free(check_next);

    printf("Placements count: %d\n", placements_count);
    return placements;
}