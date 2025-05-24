CC = gcc
CFLAGS = -Wall -Wextra -D_POSIX_C_SOURCE=199309L
TARGET = mask sender

all: $(TARGET)

mask: mask.c
	$(CC) $(CFLAGS) -o mask mask.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

clean:
	rm -f $(TARGET)

.PHONY: all clean 