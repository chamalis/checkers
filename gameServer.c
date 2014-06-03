#include "global.h"
#include "gameServer.h"
#include "board.h"
#include "move.h"

int serverSocket;					//server's socket

Position gamePosition;				//server's position
Move tempMove;						//used to store received moves


/* used by ascii server */
PlayerStruct playerOne;
PlayerStruct playerTwo;

PlayerStruct * playingPlayer;
PlayerStruct * waitingPlayer;

int numberOfGames = 1;				// 1 game by default
int swapAfterEachGame = FALSE;


