#ifndef _COMM_H
#define _COMM_H

#include "global.h"
#include "board.h"
#include "move.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h> 

/**********************************************************/
#define MAXPENDING 10
/**********************************************************/
#define NM_NEW_POSITION 1
#define NM_COLOR_W 2
#define NM_COLOR_B 3
#define NM_REQUEST_MOVE 4
#define NM_PREPARE_TO_RECEIVE_MOVE 5
#define NM_REQUEST_NAME 6
#define NM_QUIT 7
/**********************************************************/
extern char * port;
/**********************************************************/

void listenToSocket( char * port, int * mySocket );
//creates a socket and starts to listen (used by server)

int acceptConnection( int mySocket );
//accepts new connections (used by server)

void connectToTarget( char * port, char * ip, int * mySocket );
//connects to a server (used by client)

void sendMsg( int msg, int mySocket );
//sends a network message (one char)

int recvMsg( int mySocket );
//receives a network message

void sendMove( Move * moveToSend, int mySocket );
//sends a move via mySocket

void getMove( Move * moveToGet, int mySocket );
//receives a move from mySocket

void sendName( char textToSend[ MAX_NAME_LENGTH + 1 ], int mySocket );
//used to send agent's name to server

void getName( char textToGet[ MAX_NAME_LENGTH + 1 ], int mySocket );
//used to receive agent's name

void sendPosition( Position * posToSend, int mySocket );
//used to send position struct

void getPosition( Position * posToGet, int mySocket );
//used to receive position struct

#endif




