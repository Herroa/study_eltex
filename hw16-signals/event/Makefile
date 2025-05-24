CC = gcc
CFLAGS = -Wall -Wextra -D_POSIX_C_SOURCE=199309L
TARGET = receiver sender

all: $(TARGET)

receiver: reciever.c
	$(CC) $(CFLAGS) -o receiver reciever.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

clean:
	rm -f $(TARGET)

.PHONY: all clean 