all: client1 client2 server

client1: client1.c
	gcc client1.c -o client1 -lsocket

client2: client2.c
	gcc client2.c -o client2 -lsocket

server: server.c
	gcc server.c -o server -lsocket -lpthread