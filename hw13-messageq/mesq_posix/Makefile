CC = gcc
CFLAGS = -Wall -lrt

all: sender receiver

sender: sender.c
	$(CC) sender.c -o sender $(CFLAGS)

receiver: receiver.c
	$(CC) receiver.c -o receiver $(CFLAGS)

clean:
	rm -f sender receiver

.PHONY: all clean 