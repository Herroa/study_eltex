CC = gcc
CFLAGS = -Wall -Wextra

all: sender reciever

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

reciever: reciever.c
	$(CC) $(CFLAGS) -o reciever reciever.c

clean:
	rm -f sender reciever

.PHONY: all clean 