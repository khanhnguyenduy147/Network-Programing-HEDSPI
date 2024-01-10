CC = gcc
CFLAGS = -Wall -pthread
CLIENT_SRC_DIR = TCP_Client
SERVER_SRC_DIR = TCP_Server/src
CLIENT_TARGET = client
SERVER_TARGET = server

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(wildcard $(CLIENT_SRC_DIR)/*.c)
	$(CC) $(CFLAGS) -o $@ $^

$(SERVER_TARGET): $(wildcard $(SERVER_SRC_DIR)/*.c)
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f $(CLIENT_TARGET) $(SERVER_TARGET)
