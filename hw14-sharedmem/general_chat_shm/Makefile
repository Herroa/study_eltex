CC = gcc
CFLAGS = -Wall -pthread -lncurses

all: server client

server: server.c
	$(CC) server.c -o server $(CFLAGS)

client: client.c
	$(CC) client.c -o client $(CFLAGS)

clean:
	rm -f server client
