C = gcc
CFLAGS = -Wall -lpthread

all: client main SS1

client: client.c
		$(CC) -o client client.c $(CFLAGS)

main: main.c
		$(CC) -o main main.c $(CFLAGS)

server: SS1.c
		$(CC) -o server server.c $(CFLAGS)

clean:
		rm -f client main SS1 SS2 

