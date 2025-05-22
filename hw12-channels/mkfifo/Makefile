CC = gcc
CFLAGS = -Wall -Wextra -Werror
TARGETS = server client

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) -o $@ $^

client: client.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)

.PHONY: all clean 