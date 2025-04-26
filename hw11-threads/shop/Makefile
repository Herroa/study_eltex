CC = gcc
CFLAGS = -Wall -pthread
TARGETS = shop

all: $(TARGETS)

%: %.c
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -f $(TARGETS)

.PHONY: all clean