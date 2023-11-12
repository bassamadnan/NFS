C = gcc
CFLAGS = -Wall -lpthread

all: client main SS1

client: client.c
		$(CC) -o client commands.c client.c network.c file_operations.c command_execution.c $(CFLAGS)

main: main.c
		$(CC) -o main main.c  commands.c network.c file_operations.c command_execution.c  $(CFLAGS)

SS1: SS1.c
		$(CC) -o SS1 SS1.c commands.c network.c file_operations.c command_execution.c  $(CFLAGS)

clean:
		rm -f client main SS1 SS2 

