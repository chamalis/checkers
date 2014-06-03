#include "global.h"
#include "board.h"
#include "move.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**********************************************************/
Position gamePosition;		// Position we are going to use

Move moveReceived;			// temporary move to retrieve opponent's choice
Move myMove;				// move to save our choice and send it to the server

int mySocket;				// our socket

char * agentName = "TAL";	// maxlength = 8
char * ip = "127.0.0.1";	// default ip (local machine)

clock_t x_startTime;

float totalTime;			//spent for thinking - stats

/**********************************************************/


int max(int a, int b){
	if (a >= b) 
		return a;
	return b;
}

int min(int a, int b){
	if (a <= b) 
		return a;
	return b;
}

int timeUp(float limit){
	
	if((float)(clock() - x_startTime)/CLOCKS_PER_SEC > limit)
		return 1;  //time is up play your move
	
	return 0;	//go on
}

void swapMoves(Move * moves, int a, int b){
	Move temp = moves[a];
	moves[a] = moves[b];
	moves[b] = temp;
	printf("swaping moves %d %d \n", a, b);
}

/* a random move ordering - needed because a lot of successors will have the
 * same score and we dont want to move the same pieces again n again */
void moveOrdering(Move * moves, int numOfMoves){
	printf("numMoves %d \n", numOfMoves);
	int pos1,pos2, i;
	srand(time(NULL));
	for(i=0; i<numOfMoves/2; i++){
		pos1 = rand() % numOfMoves;
		pos2 = rand() % numOfMoves;
		swapMoves(moves, pos1, pos2);
	}
}	


/* How many tiles are in front of all opponent tiles (overgone - sure Score) 
 * Return how many counted in 2 last arguments (pointers) - Too expensive finally */
int howManyTilesAhead(int ourPos[12][2], int oppPos[12][2], int ourPieces, int oppPieces,
                      int * ourPawnsAhead, int * oppPawnsAhead, Position * pos){
	int i,j,y, flag;
	/*Find how many pieces are ahead of their opponents'. Firstly for us. 
	 Complexity O(ourPieces*BOARD_SIZE^2) - big overhead(evaluation called often) */
	for(i=0; i<ourPieces; i++){
		flag = TRUE;
		if(myColor == BLACK){
			for(j=1; j <= ourPos[i][0]; j++)
				for(y=ourPos[i][1] - j ; y <= ourPos[i][1] + j ; y++){
					if(y < 0 || y >= BOARD_SIZE) continue;
					if(pos->board[ourPos[i][0] - j][y] == getOtherSide(myColor)) //opponent Piece
						flag = FALSE;
				}
		}
		else if(myColor == WHITE){
			for(j=1; j < BOARD_SIZE - ourPos[i][0]; j++)
				for(y=ourPos[i][1] - j ; y <= ourPos[i][1] + j ; y++){
					if(y < 0 || y >= BOARD_SIZE) continue;
					if(pos->board[ourPos[i][0] + j][y] == getOtherSide(myColor)) //opponent Piece
						flag = FALSE;
				}
		}
		if(flag)
			*ourPawnsAhead++;
	}
	/* Now for opponent the same thing */
	for(i=0; i<oppPieces; 	i++){
		flag = TRUE;
		if(getOtherSide(myColor) == BLACK){
			for(j=1; j <= oppPos[i][0]; j++)
				for(y=oppPos[i][1] - j ; y <= oppPos[i][1] + j; y++){
					if(y < 0 || y >= BOARD_SIZE) continue;
					if(pos->board[oppPos[i][0] - j][y] == myColor) //Our Piece
						flag = FALSE;
				}
		}
		else if(getOtherSide(myColor) == WHITE){
			for(j=1; j < BOARD_SIZE - oppPos[i][0]; j++)
				for(y=oppPos[i][1] - j ; y <= oppPos[i][1] + j; y++){
					if(y < 0 || y >= BOARD_SIZE) continue;
					if(pos->board[oppPos[i][0] + j][y] == myColor) //Our Piece
						flag = FALSE;
				}
		}
		if(flag)
			*oppPawnsAhead++;
	}
}

/* Heuristic function */
int evaluate(Position * pos){
	int i, j;
	int ourPieces=0, oppPieces=0,
		ourPawnsAhead=0, oppPawnsAhead=0;
	int ourPos[12][2], oppPos[12][2];
	int ourScore, oppScore;
   // printf("EVALUATING THIS: \n");	printPosition (pos);

	/* Count pawns */
	for(i=0; i<BOARD_SIZE; i++){
		for(j=0; j<BOARD_SIZE; j++){
			if(pos->board[i][j] == myColor){
				ourPos[ourPieces][0] = i;
				ourPos[ourPieces][1] = j;
				ourPieces++;
			}
			else if(pos->board[i][j] == getOtherSide(myColor)){
				oppPos[oppPieces][0] = i;
				oppPos[oppPieces][1] = j;
				oppPieces++;
			}
		}
	}
	if(myColor == WHITE){
		ourScore = pos->score[WHITE];
		oppScore = pos->score[BLACK];
	}else{
		ourScore = pos->score[BLACK];
		oppScore = pos->score[WHITE];
	}
	
	return 2*(ourPieces - oppPieces) + 3*(ourScore - oppScore);

	//howManyTilesAhead(ourPos, oppPos, ourPieces, oppPieces, &ourPawnsAhead, &oppPawnsAhead, pos);
	//return 2*(ourPieces - oppPieces) + (ourPawnsAhead - oppPawnsAhead) + 3*(ourScore - oppScore);
}


int maximizer(Position * curPosition, int depth, int a, int b){

	int i,j, numOfMoves, u, heuristic_value;
	Move moves[24]; 	
	Position successors[24];
	
	if (depth <= 0 || timeUp (10.3))
		return evaluate(curPosition);
		
	numOfMoves = getPossibleMoves(curPosition, moves);
	if(numOfMoves == 0)
		return evaluate(curPosition);
	for(i=0; i<numOfMoves; i++)
		copy_position(&successors[i], curPosition);  //initialize them

	u = NEG_INFINITE;	
	for(i=0; i<numOfMoves; i++){	//get all successors of this node(position)
		doMove(&successors[i], &moves[i]);   //do the move on the board(child-node-successors[i])
		u = max(u, minimizer(&successors[i], depth-1, a, b)); //call the minimizer the new position
		if(u >= b) return u;	//a-b cutoff - fail high
		a = max(a,u);			//updating maximizer's best option
	}       
	return u;
}

int minimizer(Position * curPosition, int depth, int a, int b){
	int i,j, numOfMoves, u, heuristic_value;
    Move moves[24], *moveToDo=NULL;
	Position successors[24];
	
	if (depth <= 0 || timeUp (10.3) )
		return evaluate (curPosition);

	numOfMoves = getPossibleMoves(curPosition, moves);
	if(numOfMoves == 0)
		return evaluate (curPosition);
	for(i=0; i<numOfMoves; i++)
		copy_position(&successors[i], curPosition);  //initialize them

	u = INFINITE;
	for(i=0; i<numOfMoves; i++){	//get all successors of this node(position)
		doMove(&successors[i], &moves[i]);   //do the move on the board
		u = min(u, maximizer(&successors[i], depth-1, a, b));//call the maximizer at new position
		if (u<=a) return u;		//a-b cutoff - fail low
		b = min(b,u);			//updating minimizers best option
	}		
	return u;
}

/* TOP LEVEL MAXIMIZER! - decision make function */
void minimax_ab(){

	int i,j, a,b, numOfMoves, maxVal, movePos, depth;
	Move moves[24], bestMoves[24];  //cant be more...
	Position successors[24], tmpPos;

	printf("\n\nminimax_ab: ***** NEW VISIT IN 1st LEVEL **** \n");
	numOfMoves = getPossibleMoves(&gamePosition, moves); //writing the possible moves in moves[]
	if(numOfMoves == 0)
		return;
	if(numOfMoves == 1){
		copy_move(&myMove, &moves[0]);
		return;
	}
	//cause with IDS failed after 2 days of efforts we came up with this
	if(numOfMoves == 3 || numOfMoves == 4)	
		depth = 15;
	else if(numOfMoves <= 7)
		depth = 14;
	else
		depth = 13;
	
	moveOrdering(moves, numOfMoves); //random move ordering in TAL_v1.0
	
	for(i=0; i<numOfMoves; i++)
		copy_position(&successors[i], &gamePosition);  //initialize them
	
	a = NEG_INFINITE;	//a increases through time since it is maximizer's best score
	b = INFINITE;			//b decreases through time since it is minimizer's best score
	for(i=0; i<numOfMoves; i++){	//get all successors of this node(position)
		doMove(&successors[i], &moves[i]);   //do the move on the board(child-node-successors[i])		
		maxVal = minimizer(&successors[i], depth, a, b); //call the minimizer the new position
		printf("minimax_ab: maxVal for %dth move found %d\n", i+1, maxVal);

		/*Only the top-level maximizer needs to copy the best move found to do */
		if(maxVal > a){
			a = maxVal;
			movePos = i;
		}
	}   

	copy_move(&myMove, &moves[movePos]);           //the max-value move
	//printf("playing %dth move\n", movePos+1);
}

//function called when server requests a move
void make_move(){
	float timeToPlay;

	x_startTime=clock();  // start clock to count how many seconds for move
	minimax_ab ();
	timeToPlay = (float) (clock()-x_startTime) / CLOCKS_PER_SEC;  //time elapsed
	printf("played in %f\n", timeToPlay );
	if(timeToPlay > 10.0)	//never true after our trials with pentium dual core 2.1Ghz 
		printf("ALERT! we excited time limit!\n");
	totalTime += timeToPlay; //stats
}

int handle_args(int argc, char ** argv){
	int c;
	
	while( ( c = getopt ( argc, argv, "i:p:h" ) ) != -1 )
		switch( c )
		{
			case 'h':
				printf( "[-i ip] [-p port]\n" );
				return 0;
			case 'i':
				ip = optarg;
				break;
			case 'p':
				port = optarg;
				break;
			case '?':
				if( optopt == 'i' || optopt == 'p' )
					printf( "Option -%c requires an argument.\n", ( char ) optopt );
				else if( isprint( optopt ) )
					printf( "Unknown option -%c\n", ( char ) optopt );
				else
					printf( "Unknown option character -%c\n", ( char ) optopt );
				return 1;
			default:
			return 1;
		}
}

int contact_server(){
	char msg;

	msg = recvMsg( mySocket );

	switch ( msg )
	{
		case NM_REQUEST_NAME:		//server asks for our name
			sendName( agentName, mySocket );
		break;

		case NM_NEW_POSITION:		//server is trying to send us a new position
		    getPosition( &gamePosition, mySocket );
			//printPosition (&gamePosition);
		break;

		case NM_COLOR_W:			//server indorms us that we have WHITE color
			myColor = WHITE;
		break;

		case NM_COLOR_B:			//server indorms us that we have BLACK color
			myColor = BLACK;
		break;

		case NM_PREPARE_TO_RECEIVE_MOVE:	//server informs us that he will send opponent's move
			getMove( &moveReceived, mySocket );
			moveReceived.color = getOtherSide( myColor );
			doMove( &gamePosition, &moveReceived );		//play opponent's move on our position
			printf("PLAYING OPPONENT'S MOVE\n");
			printPosition( &gamePosition );
		break;

		case NM_REQUEST_MOVE:		//server requests our move
			myMove.color = myColor;

			if( !canMove( &gamePosition, myColor ) )
				myMove.tile[ 0 ][ 0 ] = -1;		//null move
			else
				make_move();
				
			sendMove( &myMove, mySocket );			//send our move
			doMove( &gamePosition, &myMove );		//play our move on our position
			printf("PLAYING OUR MOVE\n");
			printPosition( &gamePosition );
		break;

		case NM_QUIT:			//server wants us to quit...we shall obey
			printf("Total time spent thinking : %f\n", totalTime);
			close( mySocket );
		return 0;
	}
	
    return 1; //go on playing
	
}


int main( int argc, char ** argv )
{

	opterr = 0;
	totalTime = 0.0;

	if(handle_args(argc, argv) == 0)  // user just printed help ('h')
		return 0;

	connectToTarget( port, ip, &mySocket );
	
	while( contact_server() != 0);	//if server returns NM_QUIT we should exit
		
	return 0;
}

		
void make_random_move(){

	int i, j, k;
	int jumpPossible;
	int playerDirection;

	srand( time( NULL ) );
	
	if( myColor == WHITE )		// find movement's direction
		playerDirection = 1;
	else
		playerDirection = -1;

	if( canWeJump(gamePosition) )
		jumpPossible = TRUE;
	else
		jumpPossible = FALSE;
	
	while( 1 )
	{
		i = rand() % BOARD_SIZE;
		j = rand() % BOARD_SIZE;

		if( gamePosition.board[ i ][ j ] == myColor )		//find a piece of ours
		{

			myMove.tile[ 0 ][ 0 ] = i;		//piece we are going to move
			myMove.tile[ 1 ][ 0 ] = j;

			if( jumpPossible == FALSE )
			{
				myMove.tile[ 0 ][ 1 ] = i + 1 * playerDirection;
				myMove.tile[ 0 ][ 2 ] = -1;
				if( rand() % 2 == 0 )	//with 50% chance try left and then right
				{
					myMove.tile[ 1 ][ 1 ] = j - 1;
					if( isLegal( &gamePosition, &myMove ))
						break;

					myMove.tile[ 1 ][ 1 ] = j + 1;
					if( isLegal( &gamePosition, &myMove ))
						break;
				}
				else	//the other 50%...try right first and then left
				{
					myMove.tile[ 1 ][ 1 ] = j + 1;
					if( isLegal( &gamePosition, &myMove ))
						break;

					myMove.tile[ 1 ][ 1 ] = j - 1;
					if( isLegal( &gamePosition, &myMove ))
						break;
				}
			}
			else	//jump possible
			{
				if( canJump( i, j, myColor, &gamePosition ) )
				{
					k = 1;
					while( canJump( i, j, myColor, &gamePosition ) != 0 )
					{
						myMove.tile[ 0 ][ k ] = i + 2 * playerDirection;
						if( rand() % 2 == 0 )	//50% chance
						{
							if( canJump( i, j, myColor, &gamePosition ) % 2 == 1 ) //left jump possible
								myMove.tile[ 1 ][ k ] = j - 2;
							else
								myMove.tile[ 1 ][ k ] = j + 2;
						}
						else	//50%
						{
							if( canJump( i, j, myColor, &gamePosition ) > 1 )	 //right jump possible
								myMove.tile[ 1 ][ k ] = j + 2;
							else
								myMove.tile[ 1 ][ k ] = j - 2;
						}
						if( k + 1 == MAXIMUM_MOVE_SIZE )	//maximum tiles reached
							break;
						myMove.tile[ 0 ][ k + 1 ] = -1;		//maximum tiles not reached
						i = myMove.tile[ 0 ][ k ];		//we will try to jump from this point in the next loop
						j = myMove.tile[ 1 ][ k ];
						k++;
					}
					break;
				}
			}
		}
	}
}

