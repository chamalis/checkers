#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**********************************************************/
char * port = DEFAULT_PORT;		// default port

/**********************************************************/
void listenToSocket( char * port, int * mySocket )
{
	FILE * fp;

	// socket
	if( ( *mySocket = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0)
	{
		printf( "ERROR: Opening socket Failed\n" );
		exit( 1 );
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons( atoi( port ) );
	serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );

	int optval;
	optval = 1;
	setsockopt( *mySocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval );


	// bind
	if( bind( *mySocket, ( struct sockaddr* ) &serveraddr, sizeof( serveraddr ) ) < 0 )
	{
		printf( "ERROR: bind function Failed\n" );
		exit(1);
	}

	unsigned int length;

	length = sizeof( serveraddr );
	getsockname ( *mySocket, ( struct sockaddr* ) &serveraddr, &length );

	// listen
	if( listen( *mySocket, MAXPENDING ) < 0 )
	{
		printf( "ERROR: Listen failed!\n" );
		exit( 1 );
	}


}

/**********************************************************/
int acceptConnection( int mySocket )
{
	int connectedSocket;

	struct sockaddr_in connAddr;
	socklen_t clientaddrLen;

	clientaddrLen = sizeof( connAddr );

	// accept
	if( ( connectedSocket = accept( mySocket, ( struct sockaddr* ) &connAddr, &clientaddrLen ) ) < 0 )
	{
		printf( "ERROR: Accept failed!\n" );
		exit( 1 );

	}

	return connectedSocket;
}


/**********************************************************/
void connectToTarget( char * port, char * ip, int * mySocket )
{

	if( ( *mySocket = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0)
	{
		printf( "ERROR: Opening socket Failed\n" );
		exit( 1 );
	}


	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons( atoi( port ) );
	serveraddr.sin_addr.s_addr = inet_addr( ip );

	while( connect( *mySocket, ( struct sockaddr* ) &serveraddr, sizeof( serveraddr ) ) < 0 )
	{
		printf( "ERROR: Connect function Failed.. Retrying\n" );
		sleep( 1 );
	}


}



/**********************************************************/
void sendMsg( int msg, int mySocket )
{
	char msgCode;

	msgCode = ( char ) msg;

	if( send( mySocket, &msgCode, 1, 0 ) != 1 )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

}

/**********************************************************/
int recvMsg( int socket )
{
	char msg;

	if( recv( socket, &msg, 1, 0 ) != 1 )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

	return ( int ) msg;

}


/**********************************************************/
void sendMove( Move * moveToSend, int mySocket )
{
	char buffer[ 2 * MAXIMUM_MOVE_SIZE ];
	int i;

	for( i = 0; i < 2 * MAXIMUM_MOVE_SIZE; i = i + 2 )
	{
		buffer[ i ] = moveToSend->tile[ 0 ][ i / 2 ];
		buffer[ i + 1 ] = moveToSend->tile[ 1 ][ i / 2 ];
	}

	if( send( mySocket, buffer, 2 * MAXIMUM_MOVE_SIZE, 0 ) != 2 * MAXIMUM_MOVE_SIZE )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

}

/**********************************************************/
void getMove( Move * moveToGet, int mySocket )
{
	char buffer[ 2 * MAXIMUM_MOVE_SIZE ];
	int i;

	if( recv( mySocket, buffer, 2 * MAXIMUM_MOVE_SIZE, 0 ) != 2 * MAXIMUM_MOVE_SIZE )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

	for( i = 0; i < 2 * MAXIMUM_MOVE_SIZE; i = i + 2 )
	{
		moveToGet->tile[ 0 ][ i / 2 ] = buffer[ i ];
		moveToGet->tile[ 1 ][ i / 2 ] = buffer[ i + 1 ];
	}

}

/**********************************************************/
void sendName( char textToSend[ MAX_NAME_LENGTH + 1 ], int mySocket )
{
	int size;
	char size_char;

	size = strlen( textToSend );
	size_char = ( char ) size;

	if( send( mySocket, &size_char, 1, 0 ) != 1 )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

	if( send( mySocket, textToSend, size , 0 ) != size )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

}


/**********************************************************/
void getName( char textToGet[ MAX_NAME_LENGTH + 1 ], int mySocket )
{

	int size;
	char size_char;

	if( recv( mySocket, &size_char, 1, 0 ) != 1 )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

	size = ( int ) size_char;

	if( recv( mySocket, textToGet, size, 0 ) != size )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

	textToGet[ size ] = '\0';

}


/**********************************************************/
void sendPosition( Position * posToSend, int mySocket )
{
	char buffer[ BOARD_SIZE * BOARD_SIZE + 2 + 1 ];
	int i, j;

	//board
	for( i = 0; i < BOARD_SIZE; i++ )
		for( j = 0; j < BOARD_SIZE; j++ )
			buffer[ i * BOARD_SIZE + j ] =  posToSend->board[ i ][ j ];

	//score
	buffer[ BOARD_SIZE * BOARD_SIZE ] = posToSend->score[ WHITE ];
	buffer[ BOARD_SIZE * BOARD_SIZE + 1 ] = posToSend->score[ BLACK ];

	//turn
	buffer[ BOARD_SIZE * BOARD_SIZE + 2 ] = posToSend->turn;

	if( send( mySocket, buffer, BOARD_SIZE * BOARD_SIZE + 2 + 1, 0 ) != BOARD_SIZE * BOARD_SIZE + 2 + 1 )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

}

/**********************************************************/
void getPosition( Position * posToGet, int mySocket )
{
	char buffer[ BOARD_SIZE * BOARD_SIZE + 2 + 1 ];
	int i, j;

	if( recv( mySocket, buffer, BOARD_SIZE * BOARD_SIZE + 2 + 1, 0 ) != BOARD_SIZE * BOARD_SIZE + 2 + 1 )
	{
		printf( "ERROR: Network problem\n" );
		exit( 1 );
	}

	//board
	for( i = 0; i < BOARD_SIZE; i++ )
		for( j = 0; j < BOARD_SIZE; j++ )
			posToGet->board[ i ][ j ] = buffer[ i * BOARD_SIZE + j ];

	//score
	posToGet->score[ WHITE ] = buffer[ BOARD_SIZE * BOARD_SIZE ];
	posToGet->score[ BLACK ] = buffer[ BOARD_SIZE * BOARD_SIZE + 1 ];

	//turn
	posToGet->turn = buffer[ BOARD_SIZE * BOARD_SIZE + 2 ];
}






