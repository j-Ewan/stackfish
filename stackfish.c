#include "tetris_core.c"

int * get_possible_placements(struct active_piece piece, int board[BOARD_H*BOARD_W]) {
    int placements[3 * 36] = { 0 }; // 3-tuples of x, y, rot; 36 is likely the max amount of placements possible

    /** Plan:
     * init list of checked positions
     * spawn copies of main piece above board in all orientations in all columns (check each to make sure it is within bounds)
     * spawn copies for each possible movement of the piece, only proceed if it is new
     *    mark as checked
     *    if it collides, delete it
     * when no more new positions are being checked, "harddrop" all and remove dupes
     * return list
     */
}