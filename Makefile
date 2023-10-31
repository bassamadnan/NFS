CC = gcc
CFLAGS = -Wall -lpthread
SRC_DIR = Naming\ Server/src
OUTPUT_DIR = bin

all: client main server

client: $(SRC_DIR)/client.c
		$(CC) -o $(OUTPUT_DIR)/client $(SRC_DIR)/client.c $(CFLAGS)

main: $(SRC_DIR)/main.c
		$(CC) -o $(OUTPUT_DIR)/main $(SRC_DIR)/main.c $(CFLAGS)

server: $(SRC_DIR)/server.c
		$(CC) -o $(OUTPUT_DIR)/server $(SRC_DIR)/server.c $(CFLAGS)

clean:
		rm -f $(OUTPUT_DIR)/client $(OUTPUT_DIR)/main $(OUTPUT_DIR)/server

