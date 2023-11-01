CC = gcc
CFLAGS = -Wall -lpthread -w
SRC_DIR = Naming\ Server/src
OUTPUT_DIR = bin

all: client main hash2

client: $(SRC_DIR)/client.c
	$(CC) -o $(OUTPUT_DIR)/client $(SRC_DIR)/client.c $(CFLAGS)

main: $(SRC_DIR)/main.c
	$(CC) -o $(OUTPUT_DIR)/main $(SRC_DIR)/main.c $(CFLAGS)

server: $(SRC_DIR)/server.c
	$(CC) -o $(OUTPUT_DIR)/server $(SRC_DIR)/server.c $(CFLAGS)

hash2: hash/synch.c hash/main.c hash/HashMap.c
	$(CC) -o $(OUTPUT_DIR)/hash hash/synch.c hash/main.c hash/HashMap.c $(CFLAGS) 

clean:
	rm -f $(OUTPUT_DIR)/client $(OUTPUT_DIR)/main $(OUTPUT_DIR)/server $(OUTPUT_DIR)/hash
