CC = gcc
CFLAGS = -Wall -pthread
TARGETS = increment_serial increment_mutex_lock increment_mapreduce

all: $(TARGETS)

%: %.c
		$(CC) $(CFLAGS) $< -o $@

clean:
		rm -f $(TARGETS)

.PHONY: all clean