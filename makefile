all: client server

client:
	gcc libsocket.c HW5client.c -o Client

server:
	gcc libsocket.c HW5server.c -o Server

clean:
	rm Client Server tmp*
