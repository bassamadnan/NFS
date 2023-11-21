C = gcc
CFLAGS = -Werror=implicit-function-declaration -lpthread -w

all: main SS1 hash redundancy client lock

client: client.c
		$(CC) -o client commands.c client.c network.c file_operations.c command_execution.c hash.c redundancy.c lock.c $(CFLAGS)

main: main.c
		$(CC) -o main main.c  commands.c network.c file_operations.c command_execution.c hash.c redundancy.c error_codes.c lock.c $(CFLAGS)

SS1: SS1.c
		$(CC) -o ../../F1/SS1 SS1.c commands.c network.c file_operations.c command_execution.c hash.c redundancy.c lock.c $(CFLAGS) 
		$(CC) -o ../../F2/SS1 SS1.c commands.c network.c file_operations.c command_execution.c hash.c redundancy.c lock.c $(CFLAGS)
		$(CC) -o ../../F3/SS1 SS1.c commands.c network.c file_operations.c command_execution.c hash.c redundancy.c lock.c $(CFLAGS)
hash: hash.c
		$(CC) -o hash SS1.c commands.c network.c file_operations.c command_execution.c hash.c redundancy.c lock.c $(CFLAGS)
lock: lock.c
		$(CC) -o lock lock.c SS1.c commands.c network.c file_operations.c command_execution.c hash.c redundancy.c $(CFLAGS)

redundancy: redundancy.c
		$(CC) -o redundancy commands.c redundancy.c client.c network.c file_operations.c command_execution.c hash.c lock.c $(CFLAGS)		

clean:
		@rm -f client main SS1 SS2 ../../F1/SS1 ../../F2/SS1 ../../F3/SS1 redundancy lock

