all: client server

client: client.c board comm global.h
	gcc -o client client.c board.o comm.o -O3

server: server.c board comm gameServer global.h
	gcc -o server server.c board.o comm.o gameServer.o -O3

comm: comm.c comm.h global.h board move.h
	gcc -c comm.c -O3

board: board.c board.h move.h global.h
	gcc -c board.c -O3

gameServer: gameServer.c gameServer.h board.h move.h global.h
	gcc -c gameServer.c -O3

clean:
	rm -f *.o client server
