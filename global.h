#ifndef _GLOBAL_H
#define _GLOBAL_H


//#define _DEBUG_		//comment this line when you are done

#ifdef _DEBUG_
    #include <assert.h>
#else
    #define assert(p) {}
#endif


#ifndef getOtherSide
	#define getOtherSide( a ) ( 1-(a) )
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/**********************************************************/
#define MAXIMUM_MOVE_SIZE ( BOARD_SIZE - 1 ) / 2 + 1

/* The size of our board */
#define BOARD_SIZE 8

/* Values for each possible time state */
#define WHITE 0
#define BLACK 1
#define EMPTY 2
#define ILLEGAL 3

// size of our name
#define MAX_NAME_LENGTH 8

//default port for client and server
#define DEFAULT_PORT "6001"

//chefarov - TAL
#define INFINITE 65000
#define NEG_INFINITE -65000
#define NO_MOVE 59999

#ifndef MYCOLOR
#define MYCOLOR
char myColor;				// to store our color
#endif

#ifndef SWA
#define SWA
#define SWAP(t, x, y) do {t z; z = x; x=y; y=z;} while (0)
#endif

#endif
