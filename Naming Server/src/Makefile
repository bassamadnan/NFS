C = gcc
CFLAGS = -Wall -lpthread

all: client main server

client: client.c
		$(CC) -o client client.c $(CFLAGS)

main: main.c
		$(CC) -o main main.c $(CFLAGS)

server: server.c
		$(CC) -o server server.c $(CFLAGS)

clean:
		rm -f client main server

