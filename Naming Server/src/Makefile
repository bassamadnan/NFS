C = gcc
CFLAGS = -Wall -lpthread

all: client main SS SS2 hash

client: client.c
		$(CC) -o client commands.c client.c network.c file_operations.c command_execution.c hash.c $(CFLAGS)

main: main.c
		$(CC) -o main main.c  commands.c network.c file_operations.c command_execution.c hash.c $(CFLAGS)

SS: SS.c
		$(CC) -o SS SS.c commands.c network.c file_operations.c command_execution.c hash.c $(CFLAGS)

hash: hash.c
		$(CC) -o hash SS.c commands.c network.c file_operations.c command_execution.c hash.c $(CFLAGS)

SS2: SS2.c
		$(CC) -o SS2 SS2.c commands.c network.c file_operations.c command_execution.c hash.c $(CFLAGS)

clean:
		rm -f client main SS SS2 

