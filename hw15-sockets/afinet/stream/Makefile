CC=gcc
CFLAGS=-Wall -Wextra

CLIENT=client
SERVER=server

all: $(CLIENT) $(SERVER)

$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $(CLIENT) client.c

$(SERVER): server.c
	$(CC) $(CFLAGS) -o $(SERVER) server.c

clean:
	rm -f $(CLIENT) $(SERVER) 