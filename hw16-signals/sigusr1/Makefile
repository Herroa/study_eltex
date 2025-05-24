CC = gcc
CFLAGS = -Wall -Wextra
TARGETS = receiver sender

all: $(TARGETS)

receiver: reciever.c
	$(CC) $(CFLAGS) -o receiver reciever.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

clean:
	rm -f $(TARGETS) a.out

.PHONY: all clean 