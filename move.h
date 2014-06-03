#ifndef _MOVE_H
#define _MOVE_H

#include "global.h"

/**********************************************************/

/* Struct to store Move and color of the player */
typedef struct
{
	char tile[ 2 ][ MAXIMUM_MOVE_SIZE ];//(BOARD_SIZE-1) / 2 is the maximum number of jumps of a BOARD_SIZE * BOARD_SIZE board
	char color;
} Move;
/**********************************************************/


/*
Caution!

tile[0][0] and tile[1][0] holds the row and col of the piece we want to move!
All the other coordinates are the tiles that this piece will go to.

for a move to be valid the tile[0][i+1] (where tile[0][i] holds the row of the last tile of our move) must be -1.
Only exception to this is when we have to use all (( BOARD_SIZE - 1 ) / 2 + 1) tiles.


A Null move (the only legal move when we cannot move) has tile[0][0] = -1.

Server can ask for a move even we have none available.

*/


#endif
